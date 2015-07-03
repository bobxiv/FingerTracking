cmake_minimum_required (VERSION 2.8)
project (FingerTracking)
# The version number.
set (FingerTracking_VERSION_MAJOR 1)
set (FingerTracking_VERSION_MINOR 0)

#Path a includes y librerias
set(GLUT_INCLUDE "" CACHE PATH "path to GLUT includes")
set(GLUT_LIB "" CACHE PATH "path to GLUT libs")
set(OPENGL_INCLUDE "" CACHE PATH "path to OPENGL includes")
set(OPENGL_LIB "" CACHE PATH "path to OPENGL libs")
set(FREENECT_INCLUDE "" CACHE PATH "path to Freenect includes")
set(FREENECT_LIB "" CACHE PATH "path to Freenect libs")
set(PTHREADS_INCLUDE "" CACHE PATH "path to PThreads includes")
set(PTHREADS_LIB "" CACHE PATH "path to PThreads libs with library names")
set(LIBUSB_INCLUDE "" CACHE PATH "path to LIBUSB includes")
set(LIBUSB_LIB "" CACHE PATH "path to LIBUSB libs with library names")
set(BULLET_INCLUDE "" CACHE PATH "path to BULLET includes")
set(BULLET_LIB "" CACHE PATH "path to BULLET libs with library names")
set(LIBPNG_INCLUDE "" CACHE PATH "path to LIBPNG includes")
set(LIBPNG_LIB "" CACHE PATH "path to LIBPNG libs with library names")
set(GLEW_INCLUDE "" CACHE PATH "path to GLEW includes")
set(GLEW_LIB "" CACHE PATH "path to GLEW libs with library names")
set(FFTW32_INCLUDE "" CACHE PATH "path to FFTW32 includes")
set(FFTW32_LIB "" CACHE PATH "path to FFTW32 libs with library names")
set(FFTW64_INCLUDE "" CACHE PATH "path to FFTW64 includes")
set(FFTW64_LIB "" CACHE PATH "path to FFTW64 libs with library names")
set(CIMG_INCLUDE "" CACHE PATH "path to CIMG includes")
 
#Variable de con las direciones de includes a usar
set(INCLUDESDirs ${OPENGL_INCLUDE} ${GLUT_INCLUDE} ${FREENECT_INCLUDE} ${PTHREADS_INCLUDE} ${LIBUSB_INCLUDE} ${BULLET_INCLUDE} ${LIBPNG_INCLUDE} ${GLEW_INCLUDE} ${CIMG_INCLUDE} )

#Configuracion del proyecto estrictamente. Si luego hay mas de uno...

option(X_86 "Compile and link 32bits. If not 64bits"         ON)

if( X_86 )
	set(INCLUDESDirs ${INCLUDESDirs} ${FFTW32_INCLUDE} )
elseif( X_86 )
	set(INCLUDESDirs ${INCLUDESDirs} ${FFTW64_INCLUDE} )
endif( X_86 )

#Agregamos los path de includes
include_directories( ${INCLUDESDirs} )

#Proyecto de ejecutable a generar con codigos
file(GLOB FingerTracking_SOURCES *.cpp *.h)
add_executable(FingerTracking ${FingerTracking_SOURCES})


#GLUT
#Librerias a linkear
#Cada una con path completo y nombre de libreria
option(WITH_GLUT "Include GLUT in the project"         ON)

#Librerias a linkear
#Cada una con path completo y nombre de libreria
if( WITH_GLUT )
	set(LIBRARIES ${LIBRARIES} ${GLUT_LIB}glut32.lib )
endif (WITH_GLUT)

#OPENGL
option(WITH_OPENGL "Include OpenGL in the project"         ON)

#Librerias a linkear
#Cada una con path completo y nombre de libreria
if( WITH_OPENGL )

	if( X_86 )
		set(CPUARCHITECTURE )#x86
	else( X_86 )
		set(CPUARCHITECTURE x64/)#x64
	endif( X_86 )

	set(LIBRARIES ${LIBRARIES} ${OPENGL_LIB}${CPUARCHITECTURE}opengl32.lib ${OPENGL_LIB}${CPUARCHITECTURE}glu32.lib )
endif (WITH_OPENGL)

#FREENECT SYNC
option(WITH_FREENECT_SYNC "Include Freenect Sync in the project"         OFF)

if( WITH_FREENECT_SYNC )
	set(LIBRARIES ${LIBRARIES} debug ${FREENECT_LIB}Debug/freenect_sync.lib optimized ${FREENECT_LIB}Release/freenect_sync.lib )
endif( WITH_FREENECT_SYNC )

#CPUARCHITECTURE dice a que arquitectura de CPU se compilara
if( X_86 )
	set(CPUARCHITECTURE msvc/)#x86
else( X_86 )
	set(CPUARCHITECTURE msvc_x64/)#x64
endif( X_86 )

#PTHREADS
option(WITH_PTHREADS "Include PThreads in the project"         ON)

if( WITH_PTHREADS )
	set(LIBRARIES ${LIBRARIES} ${PTHREADS_LIB}pthreadVC2.lib ${PTHREADS_LIB}pthreadVCE2.lib ${PTHREADS_LIB}pthreadVSE2.lib )
endif( WITH_PTHREADS )

#BULLET
option(WITH_BULLET "Include Bullet physic engine in the project"         ON)

if( WITH_BULLET )
	if( MSVC_VERSION MATCHES 1500 )#Version 9 de VS
		set(VSDIR vc2008/)
	elseif( MSVC_VERSION MATCHES 1600 )#Version 10 de VS
		set(VSDIR vc2010/)
	endif( MSVC_VERSION MATCHES 1500 )
	
	set(LIBRARIES ${LIBRARIES} debug ${BULLET_LIB}${VSDIR}LinearMath_debug.lib optimized ${BULLET_LIB}${VSDIR}LinearMath.lib debug ${BULLET_LIB}${VSDIR}BulletDynamics_debug.lib optimized ${BULLET_LIB}${VSDIR}BulletDynamics.lib debug ${BULLET_LIB}${VSDIR}BulletCollision_debug.lib optimized ${BULLET_LIB}${VSDIR}BulletCollision.lib )
endif( WITH_BULLET )

#LIBPNG
set(LIBRARIES ${LIBRARIES} debug ${LIBPNG_LIB}${VSDIR}libpngd-s.lib optimized ${LIBPNG_LIB}${VSDIR}libpng-s.lib debug ${LIBPNG_LIB}${VSDIR}zlibd-s.lib optimized ${LIBPNG_LIB}${VSDIR}zlib-s.lib )

#GLEW
option(STATIC_GLEWLIB "Link with static glew library in the project"         ON)

if( STATIC_GLEWLIB )
	set(LIBRARIES ${LIBRARIES} debug ${GLEW_LIB}glew32sd.lib optimized ${GLEW_LIB}glew32s.lib )
elseif( STATIC_GLEWLIB )
	set(LIBRARIES ${LIBRARIES} debug ${GLEW_LIB}glew32d.lib optimized ${GLEW_LIB}glew32.lib )
endif( STATIC_GLEWLIB)

#FFTW
if( X_86 )
	set(LIBRARIES ${LIBRARIES} ${FFTW32_LIB}libfftw3-3.lib ${FFTW32_LIB}libfftw3f-3.lib ${FFTW32_LIB}libfftw3l-3.lib )
elseif( X_86 )
	set(LIBRARIES ${LIBRARIES} ${FFTW64_LIB}libfftw3-3.lib ${FFTW64_LIB}libfftw3f-3.lib ${FFTW64_LIB}libfftw3l-3.lib )
endif( X_86 )

target_link_libraries(FingerTracking ${LIBRARIES} debug ${FREENECT_LIB}Debug/freenect.lib optimized ${FREENECT_LIB}Release/freenect.lib ${LIBUSB_LIB}${CPUARCHITECTURE}libusb.lib )