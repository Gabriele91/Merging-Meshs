#ifndef BLEND_H
#define BLEND_H

#include <Config.h>

namespace Easy3D{
	//blend
	namespace BLEND{

		enum{
			ONE=GL_ONE,
			ZERO=GL_ZERO
		};

		struct ONE{
			struct MINUS{
				struct DST{
					enum{
						COLOR=GL_ONE_MINUS_DST_COLOR,
                        ALPHA=GL_ONE_MINUS_DST_ALPHA
					};
				};
				struct SRC{
					enum{
						COLOR=GL_ONE_MINUS_SRC_COLOR,
						ALPHA=GL_ONE_MINUS_SRC_ALPHA
					};
				};				
			};
		};
	
		struct DST{	
			enum{	
			 COLOR=GL_DST_COLOR,
			 ALPHA=GL_DST_ALPHA
			};
		};

		struct SRC{	
			enum{	
				COLOR=GL_SRC_COLOR,
				ALPHA=GL_SRC_ALPHA
			};
			struct ALPHA{	
				enum{	
					SATURATE=GL_SRC_ALPHA_SATURATE
				};
			};
		};

       
	};
};

#endif