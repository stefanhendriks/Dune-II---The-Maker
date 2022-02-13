/* AllegroFont - a wrapper for FreeType 2 */
/* to render TTF and other font formats with Allegro */

            
/* FreeType 2 is copyright (c) 1996-2000 */
/* David Turner, Robert Wilhelm, and Werner Lemberg */
/* AllegroFont is copyright (c) 2001, 2002 Javier Gonzãlez */
/* Enhanced by Chernsha since 2004 year */

/* See FTL.txt (FreeType Project License) for license */


#ifndef ALFONT_H
#define ALFONT_H


#ifdef ALFONT_WINDOWS	//run in WINDOWS
#define Encode_Code "chinese-traditional"
#endif
#ifdef ALFONT_DOS	    //run in DOS
#define Encode_Code "BIG5"
#endif
#ifdef ALFONT_LINUX		//run in LINUX
#define Encode_Code "zh_TW.Big5"
#endif


#include <allegro.h>

#include "alfontdll.h"


#ifdef __cplusplus
extern "C" {
#endif


/* common define */

#define ALFONT_MAJOR_VERSION        2
#define ALFONT_MINOR_VERSION        0
#define ALFONT_SUB_VERSION          9
#define ALFONT_VERSION_STR          "2.0.9"
#define ALFONT_DATE_STR             "31/01/2009"
#define ALFONT_DATE                 20090131    /* yyyymmdd */

/* error codes */

#define ALFONT_OK                   0
#define ALFONT_ERROR                -1


/* includes */

#include <allegro.h>


/* structs */
typedef struct ALFONT_FONT ALFONT_FONT;


/* API */

ALFONT_DLL_DECLSPEC int alfont_init(void);
ALFONT_DLL_DECLSPEC void alfont_exit(void);

ALFONT_DLL_DECLSPEC ALFONT_FONT *alfont_load_font(const char *filepathname);
ALFONT_DLL_DECLSPEC ALFONT_FONT *alfont_load_font_from_mem(const char *data, int data_len);
ALFONT_DLL_DECLSPEC void alfont_destroy_font(ALFONT_FONT *f);

ALFONT_DLL_DECLSPEC int alfont_set_font_size(ALFONT_FONT *f, int h);
ALFONT_DLL_DECLSPEC int alfont_get_font_height(ALFONT_FONT *f);

ALFONT_DLL_DECLSPEC int alfont_text_mode(int mode);

ALFONT_DLL_DECLSPEC void alfont_set_language(ALFONT_FONT *f, const char *language);				//Sets Language
ALFONT_DLL_DECLSPEC char *alfont_get_language(ALFONT_FONT *f);									//Returns Language
ALFONT_DLL_DECLSPEC void alfont_set_convert(ALFONT_FONT *f, int type);							//Sets Code Convert(Please Use TYPE_WIDECHAR for ASCII to UNICODE,TYPE_MULTIBYTE for UNICODE to ASCII)
ALFONT_DLL_DECLSPEC int alfont_get_convert(ALFONT_FONT *f);										//Returns Code Convert(Return TYPE_WIDECHAR for ASCII to UNICODE,TYPE_MULTIBYTE for UNICODE to ASCII)
ALFONT_DLL_DECLSPEC void alfont_set_font_style(ALFONT_FONT *f, int style);						//Sets Font Style(Please Use STYLE_STANDARD,STYLE_ITALIC,STYLE_BOLD or STYLE_BOLDITALIC)
ALFONT_DLL_DECLSPEC int alfont_get_font_style(ALFONT_FONT *f);									//Returns Font Style(Please Use STYLE_STANDARD,STYLE_ITALIC,STYLE_BOLD or STYLE_BOLDITALIC)
ALFONT_DLL_DECLSPEC void alfont_set_font_underline(ALFONT_FONT *f, int underline);				//Sets Font underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_font_underline(ALFONT_FONT *f);								//Returns Font underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC void alfont_set_font_underline_right(ALFONT_FONT *f, int underline_right);	//Sets Extend right underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_font_underline_right(ALFONT_FONT *f);						//Returns Extend right underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC void alfont_set_font_underline_left(ALFONT_FONT *f, int underline_left);	//Sets Extend left underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_font_underline_left(ALFONT_FONT *f);							//Returns Extend left underline(TRUE/FALSE)
ALFONT_DLL_DECLSPEC void alfont_set_font_background(ALFONT_FONT *f, int background);			//Sets Font Background Color(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_font_background(ALFONT_FONT *f);								//Returns Font Background Color(TRUE/FALSE)
ALFONT_DLL_DECLSPEC void alfont_set_font_transparency(ALFONT_FONT *f, int transparency);		//Sets Font transparency(0-255)
ALFONT_DLL_DECLSPEC int alfont_get_font_transparency(ALFONT_FONT *f);							//Returns Font transparency(0-255)
ALFONT_DLL_DECLSPEC void alfont_set_autofix(ALFONT_FONT *f, int autofix);						//Sets autofix(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_autofix(ALFONT_FONT *f);										//Returns autofix(TRUE/FALSE)
ALFONT_DLL_DECLSPEC void alfont_set_precedingchar(ALFONT_FONT *f, int precedingchar);			//Sets precedingchar
ALFONT_DLL_DECLSPEC int alfont_get_precedingchar(ALFONT_FONT *f);								//Returns precedingchar

ALFONT_DLL_DECLSPEC int alfont_ugetc(ALFONT_FONT *f, const char *s);							//Returns the character pointered by `s' in the current encoding format
ALFONT_DLL_DECLSPEC int alfont_ugetx(ALFONT_FONT *f, char **s);									//Returns the character pointered by `s' in the current encoding format, and advances the pointer to the next character after the one just returned
ALFONT_DLL_DECLSPEC int alfont_ugetxc(ALFONT_FONT *f, const char **s);							//Returns the character pointered by `s' in the current encoding format, and advances the pointer to the next character after the one just returned

ALFONT_DLL_DECLSPEC void alfont_get_string(ALFONT_FONT *f, const char *s , char **out);			//Gets the converted string pointered by `s' in the current encoding format

ALFONT_DLL_DECLSPEC void alfont_set_font_outline_top(ALFONT_FONT *f, int w);					//Sets Font top outline width
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_top(ALFONT_FONT *f);							//Returns Font top outline width
ALFONT_DLL_DECLSPEC void alfont_set_font_outline_bottom(ALFONT_FONT *f, int w);					//Sets Font bottom outline width
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_bottom(ALFONT_FONT *f);							//Returns Font bottom outline width
ALFONT_DLL_DECLSPEC void alfont_set_font_outline_left(ALFONT_FONT *f, int w);					//Sets Font left outline width
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_left(ALFONT_FONT *f);							//Returns Font left outline width
ALFONT_DLL_DECLSPEC void alfont_set_font_outline_right(ALFONT_FONT *f, int w);					//Sets Font right outline width
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_right(ALFONT_FONT *f);							//Returns Font right outline width
ALFONT_DLL_DECLSPEC void alfont_set_font_outline_color(ALFONT_FONT *f, int c);					//Sets Font outline color
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_color(ALFONT_FONT *f);							//Returns Font outline color
ALFONT_DLL_DECLSPEC void alfont_set_font_outline_hollow(ALFONT_FONT *f, int hollow);			//Sets Font hollow(TRUE/FALSE)
ALFONT_DLL_DECLSPEC int alfont_get_font_outline_hollow(ALFONT_FONT *f);							//Returns Font hollow(TRUE/FALSE)

ALFONT_DLL_DECLSPEC void alfont_textout_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
ALFONT_DLL_DECLSPEC void alfont_textout_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

ALFONT_DLL_DECLSPEC void alfont_textout_centre_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout_centre(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
ALFONT_DLL_DECLSPEC void alfont_textout_centre_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

ALFONT_DLL_DECLSPEC void alfont_textout_right_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout_right(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color);
ALFONT_DLL_DECLSPEC void alfont_textout_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);
ALFONT_DLL_DECLSPEC void alfont_textout_right_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int bg);

ALFONT_DLL_DECLSPEC void alfont_textprintf(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

ALFONT_DLL_DECLSPEC void alfont_textprintf_centre(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_centre_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_centre_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

ALFONT_DLL_DECLSPEC void alfont_textprintf_right(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_right_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_right_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);
ALFONT_DLL_DECLSPEC void alfont_textprintf_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int bg, const char *format, ...);

ALFONT_DLL_DECLSPEC int alfont_text_height(ALFONT_FONT *f);
ALFONT_DLL_DECLSPEC int alfont_text_length(ALFONT_FONT *f, const char *str);
ALFONT_DLL_DECLSPEC int alfont_char_length(ALFONT_FONT *f, int character);
ALFONT_DLL_DECLSPEC int alfont_text_count(ALFONT_FONT *f, const char *str);						//Returns char Count(ASCII To Unicode)

ALFONT_DLL_DECLSPEC int alfont_need_uconvert(ALFONT_FONT *f, const char *str);					//Returns TRUE if TYPE_WIDECHAR conversion is required or FALSE otherwise. 

ALFONT_DLL_DECLSPEC int alfont_is_fixed_font(ALFONT_FONT *f);
ALFONT_DLL_DECLSPEC int alfont_is_scalable_font(ALFONT_FONT *f);

ALFONT_DLL_DECLSPEC void alfont_set_font_fixed_width(ALFONT_FONT *f, int fixed_width);			//Sets Font fixed width(TRUE/FALSE)

ALFONT_DLL_DECLSPEC const int *alfont_get_available_fixed_sizes(ALFONT_FONT *f);
ALFONT_DLL_DECLSPEC int alfont_get_nof_available_fixed_sizes(ALFONT_FONT *f);

ALFONT_DLL_DECLSPEC int alfont_get_char_extra_spacing(ALFONT_FONT *f);
ALFONT_DLL_DECLSPEC void alfont_set_char_extra_spacing(ALFONT_FONT *f, int spacing);

#ifdef __cplusplus
}
#endif

#endif
//
#define TYPE_NOSWITCH 0   //No Switch
#define TYPE_MULTIBYTE 1  //Local Code(convert Unicode Code To ASCII Code)
#define TYPE_WIDECHAR 2   //Unicode Code(convert ASCII Code To UNICODE Code)

//
#define STYLE_STANDARD 0     //Standard style font
#define STYLE_ITALIC 1	     //Italic style font
#define STYLE_BOLD 2         //Bold style font
#define STYLE_BOLDITALIC 3   //BoldItalic style font

/*Win32 Language Strings

Chinese "chinese" 
Chinese(simplified) "chinese-simplified" or "chs" 
Chinese(traditional) "chinese-traditional" or "cht" 
Czech "csy" or "czech" 
Danish "dan"or "danish" 
Dutch(Belgian) "belgian", "dutch-belgian", or "nlb" 
Dutch(default) "dutch" or "nld" 
English(Australian) "australian", "ena", or "english-aus" 
English(Canadian) "canadian", "enc", or "english-can" 
English(default) "english" 
English(New Zealand) "english-nz" or "enz" 
English(UK) "eng", "english-uk", or "uk" 
English(USA) "american", "american english", "american-english", "english-american", "english-us", "english-usa", "enu", "us", or "usa" 
Finnish "fin" or "finnish" 
French(Belgian) "frb" or "french-belgian" 
French(Canadian) "frc" or "french-canadian" 
French(default) "fra"or "french" 
French(Swiss) "french-swiss" or "frs" 
German(Austrian) "dea" or "german-austrian" 
German(default) "deu" or "german" 
German(Swiss) "des", "german-swiss", or "swiss" 
Greek "ell" or "greek" 
Hungarian "hun" or "hungarian" 
Icelandic "icelandic" or "isl" 
Italian(default) "ita" or "italian" 
Italian(Swiss) "italian-swiss" or "its" 
Japanese "japanese" or "jpn" 
Korean "kor" or "korean" 
Norwegian(Bokmal) "nor" or "norwegian-bokmal" 
Norwegian(default) "norwegian" 
Norwegian(Nynorsk) "non" or "norwegian-nynorsk" 
Polish "plk" or "polish" 
Portuguese(Brazilian) "portuguese-brazilian" or "ptb" 
Portuguese(default) "portuguese" or "ptg" 
Russian(default) "rus" or "russian" 
Slovak "sky" or "slovak" 
Spanish(default) "esp" or "spanish" 
Spanish(Mexican) "esm" or "spanish-mexican" 
Spanish(Modern) "esn" or "spanish-modern" 
Swedish "sve" or "swedish" 
Turkish "trk" or "turkish" 

*/

/*Linux Language Strings
Afar                    aa
Abkhazian               ab
Afrikaans               af
Amharic                 am
Arabic                  ar
Assamese                as
Aymara                  ay
Azerbaijani             az
                        
Bashkir                 ba
Byelorussian            be
Bulgarian               bg
Bihari                  bh
Bislama                 bi
Bengali; Bangla         bn
Tibetan                 bo
Breton                  br

                        C
Catalan                 ca
Corsican                co
Czech                   cs cs_CZ
Welsh                   cy
                        
Danish                  da da_DK
German                  de de_AT de_DE
Bhutani                 dz
                        
Greek                   el
English                 en en_AU en@boldquot en_CA en_GB en@IPA en@quot en_SE en_US
Esperanto               eo
Spanish                 es es_DO es_ES es_GT es_HN es_MX es_NI es_PA es_PE es_SV
Estonian                et et_EE
Basque                  eu
			eu_ES
                        
Persian                 fa
Finnish                 fi fi_FI
Fiji                    fj
Faroese                 fo
French                  fr fr_FR
Frisian                 fy
                        
Irish                   ga
Scots Gaelic            gd
Galician                gl
Guarani                 gn
Gujarati                gu
                        
Hausa                   ha
Hebrew (formerly iw)    he
Hindi                   hi
Croatian                hr
Hungarian               hu
Armenian                hy
                        
Interlingua             ia
Indonesian (formerly in)id
Interlingue             ie
Inupiak                 ik
Icelandic               is
Italian                 it
Inuktitut               iu
                        
Japanese                ja ja_JP.eucJP ja_JP.SJIS
Javanese                jw
                        
Georgian                ka
Kazakh                  kk
Greenlandic             kl
Cambodian               km
Kannada                 kn
Korean                  ko
Kashmiri                ks
Kurdish                 ku
Kirghiz                 ky
                        
Latin                   la
Lingala                 ln
Laothian                lo
Lithuanian              lt
			lug
Latvian, Lettish        lv
                        
Malagasy                mg
Maori                   mi
Macedonian              mk
Malayalam               ml
Mongolian               mn
Moldavian               mo
Marathi                 mr
Malay                   ms
Maltese                 mt
Burmese                 my
                        
Nauru                   na
			nb_NO
Nepali                  ne
Dutch                   nl
			nn_NO
Norwegian               no no_NY
                        
Occitan                 oc
(Afan) Oromo            om
Oriya                   or
                        
Punjabi                 pa
Polish                  pl
Pashto, Pushto          ps
Portuguese              pt pt_BR pt_PT
                        
Quechua                 qu
                        
Rhaeto-Romance          rm
Kirundi                 rn
Romanian                ro
Russian                 ru ru_RU.cp1251 ru_RU.koi8r
Kinyarwanda             rw
                        
Sanskrit                sa
Sindhi                  sd
Sangho                  sg
Serbo-Croatian          sh
Sinhalese               si
Slovak                  sk
Slovenian               sl
Samoan                  sm
Shona                   sn
Somali                  so
Albanian                sq
Serbian                 sr
Siswati                 ss
Sesotho                 st
Sundanese               su
Swedish                 sv
Swahili                 sw
                        
Tamil                   ta
Telugu                  te
Tajik                   tg
Thai                    th
Tigrinya                ti
Turkmen                 tk
Tagalog                 tl
Setswana                tn
Tonga                   to
Turkish                 tr
Tsonga                  ts
Tatar                   tt
Twi                     tw
                        
Uighur                  ug
Ukrainian               uk uk_UA.cp1251
Urdu                    ur
Uzbek                   uz
                        
Vietnamese              vi
Volapuk                 vo
                        
Wolof                   wo
                        
Xhosa                   xh
                        
Yiddish (formerly ji)   yi
Yoruba                  yo
                        
Zhuang                  za
Chinese                 zh
Chinese(Hong kong)	zh_HK
Chinese(simple)		zh_CN zh_CN.GB2312
Chinese(traditional)	zh_TW zh_TW.Big5
Zulu                    zu
*/

/*DOS Language Strings
European languages 
ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh 

Semitic languages 
ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic} 

Japanese 
EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1 

Chinese 
EUC-CN, HZ, GBK, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, ISO-2022-CN, ISO-2022-CN-EXT

ps. Chinese(traditional) : BIG5 , Chinese(simplified) : GBK

Korean 
EUC-KR, CP949, ISO-2022-KR, JOHAB 

Armenian 
ARMSCII-8 

Georgian 
Georgian-Academy, Georgian-PS 

Tajik 
KOI8-T 

Thai 
TIS-620, CP874, MacThai 

Laotian 
MuleLao-1, CP1133 

Vietnamese 
VISCII, TCVN, CP1258 

Platform specifics 
HP-ROMAN8, NEXTSTEP 

Full Unicode 
UTF-8 
UCS-2, UCS-2BE, UCS-2LE 
UCS-4, UCS-4BE, UCS-4LE 
UTF-16, UTF-16BE, UTF-16LE 
UTF-32, UTF-32BE, UTF-32LE 
UTF-7 
C99, JAVA 

Full Unicode, in terms of uint16_t or uint32_t (with machine dependent endianness and alignment) 
UCS-2-INTERNAL, UCS-4-INTERNAL 

European languages 
CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125} 

Semitic languages 
CP864 

Japanese 
EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3 

Turkmen 
TDS565 

Platform specifics 
RISCOS-LATIN1 
*/