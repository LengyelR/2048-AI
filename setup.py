from distutils.core import setup, Extension
from Cython.Build import cythonize
import numpy as np


ext = Extension("simulator",
        sources=["simulator.pyx",

                "move/bit_move.cpp",
                "move/encoding.cpp",
				"move/vector_move.cpp",

                "simulator/game.cpp",

                "mcts/mcts.cpp",
                "mcts/algorithm.cpp",
                "mcts/graph.cpp"],

        include_dirs=["mcts", 
                "simulator", 
                "move", 
                "encoding", 
                "cuda", 
                np.get_include()],

        libraries=["cuda/x64/Release/cuda_move"],

        extra_compile_args=["/std:c++17", "/MT"],
        extra_link_args=[],
        language='c++')
                
setup(ext_modules=cythonize([ext]))
