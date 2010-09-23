#ifndef __CHKERROR_H__
#define __CHKERROR_H__

inline void CHECK( int tf, char *msg )
{
	if ( tf )
	{
		G_deinit();
		cout << "\rerror: " << msg << "                                                                                                             " << endl;
		exit( 1 );
	}
}

inline void _CHECK( int tf, char *msg )
{
	if ( tf )
	{
		G_deinit();
		cout << "\rerror: " << msg << "                                                                                                            " << endl;
		exit( 1 );
	}
}

#ifndef __DEBUG__
  #define CHECK _CHECK
#endif

#endif
