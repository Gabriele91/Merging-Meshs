import sys
from os.path import isfile
name=raw_input("Name files:")
nameupper=name.upper()

def main():
    #make h file
    if not isfile("appsrc/"+name+".h"):
        ofile = open("appsrc/"+name+".h","w")
        ofile.write("#ifndef "+nameupper+"_H\n")
        ofile.write("#define "+nameupper+"_H\n\n")
        ofile.write("namespace Easy3D {\n\n\n\n")
        ofile.write("};\n\n")
        ofile.write("#endif")
        ofile.close()
    else:
        print("appsrc/"+name+".h, olready exist")
    #make cppfile
    if not isfile("appsrc/"+name+".cpp"):
        ofile = open("appsrc/"+name+".cpp","w")
        ofile.write("#include <stdafx.h>\n")
        ofile.write("#include <"+name+".h>\n")
        ofile.write("///////////////////////\n")
        ofile.write("using namespace Easy3D;\n")
        ofile.write("///////////////////////\n\n")
        ofile.close()
    else:
        print("appsrc/"+name+".cpp, olready exist")

if __name__ == "__main__":
    main()