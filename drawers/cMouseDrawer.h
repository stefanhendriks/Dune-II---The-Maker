/* 
 * File:   cMouseDrawer.h
 * Author: el.anormal
 *
 * Created Oct 23, 2010
 */

#ifndef CMOUSEDRAWER_H
#define	CMOUSEDRAWER_H

class cMouseDrawer {
public:
    cMouseDrawer();
    cMouseDrawer(const cMouseDrawer& orig);
    virtual ~cMouseDrawer();
    void draw();
private:

};

#endif	/* CMOUSEDRAWER_H */

