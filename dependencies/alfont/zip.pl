#!/usr/bin/perl

# make a zip / tar archive.
# by default, creates both at once.

use strict;
use warnings;
use Archive::Zip qw( :ERROR_CODES :CONSTANTS );
use Archive::Tar;

my $fnConf = "zip.conf";

my $zipbasename = undef;
my $zipbasedir = undef;
my $version = undef; 
my @zips = ();

# read config file

if (! -r "zip.conf")
{
	die "zip.conf file not found in current dir!\n";
}

open INCONF, "zip.conf" or die "Couldn't open zip.conf!, $!";

my $section = undef;
while (my $line = <INCONF>)
{
	chomp $line;
	if ($line =~ /^\s*$/)
	{
		#ignore whitespace
	}
	elsif ($line =~ /^#/)
	{
		#ignore comment line
	}
	elsif ($line =~ /(\w+)=(.*)/)
	{
		my $key = $1;
		my $value = $2;
		if (defined $section)
		{
			if ($key eq "post")
			{
				$zips[-1]{post} = $value;
			}
			elsif ($key eq "list")
			{
				$zips[-1]{list} = $value;
			}
			else
			{
				die "key '$key' not allowed in subsection";
			}
		}
		else
		{			
			if ($key eq "basename")
			{
				$zipbasename = $value;
			}
			elsif ($key eq "basedir")
			{
				$zipbasedir = $value;
			}
			elsif ($key eq "version")
			{
				$version = $value;
			}
			else
			{
				die "key '$key' not allowed in main section";
			}
		}
	}
	elsif ($line =~ /\[(.*)\]/)
	{
		$section = $1;
		push @zips, {};
	}
	else
	{
		die "Syntax error in 'zip.conf'";
	}
}

if (!defined $zipbasename) { die "Error: property 'zipbasename' not defined in zip.conf\n"; }
if (!defined $zipbasedir) { die "Error: property 'zipbasedir' not defined in zip.conf\n"; }
if (!defined $version) { die "Error: property 'version' not defined in zip.conf\n"; }
if (@zips < 1) { die "No sections defined in 'zip.conf'"; }

for my $zipinfo (@zips)
{
	open INFILE, $$zipinfo{list} or die "Couldn't open " . $$zipinfo{list} . ", $!";
	my @lines = <INFILE>;
	chomp @lines;
	close INFILE;
	
	my @files;
	for (@lines)
	{
		my @temp = grep { -r } glob ($_);
		unless (@temp)
		{
			warn ("Warning: pattern '$_' in $$zipinfo{list} didn't match any files\n");
		}
		push @files, @temp;
	}	
	
	# write files to ZIP
	my $fnOutZip = $zipbasename . "-" . $version . "_" . $$zipinfo{post} . ".zip";
	unlink $fnOutZip;
	my $zip = Archive::Zip->new();	
	for my $file (@files)
	{
		my $member = $zip->addFile( $file, $zipbasedir . "/" . $file );
	}
	die 'write error' unless $zip->writeToFileNamed($fnOutZip) == AZ_OK;
	print scalar @files, " files written to $fnOutZip\n";
	
	# now do the same thing for TAR
	my $fnOutTar = $zipbasename . "-" . $version . "_" . $$zipinfo{post} . ".tar.gz";
	unlink $fnOutTar;
	my $tar = Archive::Tar->new();
	$tar->add_files (@files);
	die 'write error' unless $tar->write ($fnOutTar, 5, $zipbasedir);
	print scalar @files, " files written to $fnOutTar\n";
}
