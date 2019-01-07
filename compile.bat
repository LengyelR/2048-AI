if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
)


nvcc -gencode=arch=compute_61,code=\"sm_61,compute_61\" --use-local-env -o cuda\x64\Release\cuda_move.lib cuda\cuda_move.cu -x cu -I"simulator" -I"move" -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.0\include" -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.0\include" --keep-dir x64\Release --machine 64 --compile -cudart static -Xcompiler "/EHsc /nologo /O2 /FS /MT"

python setup.py build_ext --inplace -I"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.0\include" -lcudart -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v10.0\lib\x64" --force clean
