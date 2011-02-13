/* 

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2009 (c) code by Stefan Hendriks

  */

class cAtreidesMentat : public cMentat {
private:	
	void draw_mouth();
	void draw_eyes();
	void draw_other();

public:
	cAtreidesMentat();
	~cAtreidesMentat();
	
	void draw();
};

