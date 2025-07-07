 /*********************************************** 
  *    •ûŒüŠpŒvŽZ
  *    ƒRƒEƒi[ŒvŽZ                             *
  *                 kikoun.h                    *
  *                     Ô¼@“N’j              * 
  ***********************************************/ 

#ifndef Private
#define Private
#endif

Private double Deg_Rad( double deg );
Private double Deg_Rad( double deg )
{
	double	rad;
        	rad = deg * fc_piover180;
		return( rad );
}
Private double Rad_Deg( double rad );
Private double Rad_Deg( double rad )
{
	double	deg;
        	deg = rad * fc_180overpi;
		return( deg );
}

Private double dkisan(double y1, double x1, double y2, double x2);

  /* -------------------------- ‰ñ“]Šp ----------------------------*/
Private double dkisan(double y1, double x1, double y2, double x2)
{
      double x0,y0,tt,ta;
    x0=x2-x1;  y0=y2-y1;
    if ( x0 == 0.0  ||  y0 == 0.0 )
        {
          if ( x0 == 0.0  &&  y0 > 0.0 ){  tt=90.0;}
          if ( x0 == 0.0  &&  y0 < 0.0 ){  tt=270.0;}
          if ( x0 > 0.0  &&  y0 == 0.0 ){  tt=0.0;}
          if ( x0 < 0.0  &&  y0 == 0.0 ){  tt=180;}
          }
        else
         {
//           ta=fc_180overpi*(atan(y0/x0));
	     ta = Rad_Deg( atan(y0/x0) );
           if ( x0 > 0.0  &&  y0 > 0.0 ){  tt=ta;}
           if ( x0 < 0.0 )              {  tt=ta+180.0;}
           if ( x0 > 0.0  &&  y0 < 0.0 ){  tt=ta+360.0;}
           }
    return(tt);
}

 /********************************************************************* 
  *           xy_kis.h                                                * 
  *                             1990-12-14                            * 
  *                                    Ô¼@“N’j                     * 
  *********************************************************************/
Private double x_kis(double x, double deg, double s);
Private double y_kis(double y, double deg, double s);

/*-------------------------------------------------------------------*/

Private double x_kis(double x, double deg, double s)
{
       double x0;
        x0=x+sin(deg * fc_piover180)*s;
         return(x0);
}
Private double y_kis(double y, double deg, double s)
{
        double y0;
         y0=y+cos(deg * fc_piover180)*s;
          return(y0);
}


Private double kyori( double x1, double y1, double x2, double y2 );

Private double kyori( double x1, double y1, double x2, double y2 )
{
	double kyor;

	kyor = sqrt(((x1-x2)*(x1-x2))+((y1-y2)*(y1-y2)));
	return( kyor );

}


/*----------------------------------------------------------------------+
|                                                                       |
| name		Matrix			                                |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
| name		Get_LDeg 	                                        |
+----------------------------------------------------------------------*/
Private double	Get_LDeg( Dpoint3d *points, int stnum, int ennum );
Private double	Get_LDeg( Dpoint3d *points, int stnum, int ennum )
{
	double	lDeg;

		lDeg = dkisan( points[stnum].y, points[stnum].x, points[ennum].y, points[ennum].x );


		return( lDeg );
}
/*----------------------------------------------------------------------+
| name		Deg_rMatrix 	                                        |
+----------------------------------------------------------------------*/
Private void Deg_rMatrix( RotMatrix *rMatrix, double deg );
Private void Deg_rMatrix( RotMatrix *rMatrix, double deg )
{
	double	  rad;

		rad = Deg_Rad( deg );
		mdlRMatrix_fromAngle( rMatrix, rad );

}
