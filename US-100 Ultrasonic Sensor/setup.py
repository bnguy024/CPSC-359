from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
#Code taken from tutorial 5 us100
ext_modules = [ 
	Extension("game",
				sources = ["gamewrapper.pyx", 'game.c'],
				include_dirs = ["/opt/vc/include"],
				libraries = [ "bcm_host"],
				library_dirs = ["/opt/vc/lib"]
		)
			
]

setup(
	name = "game",
	ext_modules = cythonize( ext_modules )
)
