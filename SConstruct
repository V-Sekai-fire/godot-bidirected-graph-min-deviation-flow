#!/usr/bin/env python
import os
import sys

from methods import print_error

libname = "bimdf"
projectdir = "godot-bigraph-min-deviation-flow"

localEnv = Environment(tools=["default"], PLATFORM="")
localEnv["disable_exceptions"] = False

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

submodule_initialized = False
dir_name = 'godot-cpp'
if os.path.isdir(dir_name):
    if os.listdir(dir_name):
        submodule_initialized = True

if not submodule_initialized:
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

# Setup libsatsuma
libsatsuma_path = "thirdparty/libsatsuma/src/libsatsuma"
libsatsuma_sources = []
libsatsuma_dirs = [
    "Problems", "Extra", "IO", "Reductions",
]
for subdir in libsatsuma_dirs:
    libsatsuma_sources += Glob(f"{libsatsuma_path}/{subdir}/*.cc")

# Specific files from Solvers directory
specific_solver_files = [
    "BiMDFDoubleCover.cc", "BiMDFGuess.cc", "BiMDFRefinement.cc",
    "EvenBiMDF.cc", "Matching.cc", "MCF.cc", "TJoinMST.cc", "OrientBinet.cc"
]
for file in specific_solver_files:
    libsatsuma_sources.append(f"{libsatsuma_path}/Solvers/{file}")

env.Append(CPPPATH=[
    libsatsuma_path,
    "thirdparty/libsatsuma/src",
    "thirdparty/libsatsuma/external/lemon",
    "thirdparty/libsatsuma/external/timekeeper/src",
    f"{libsatsuma_path}/Solvers",
    "#modules/bimdf"
])

if env["CC"] == "cl":
    env.Append(CXXFLAGS=['/std:c++2a'])
else:
    env.Append(CXXFLAGS=['-std=c++2a'])

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp") + libsatsuma_sources

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

file = "{}{}{}".format(libname, env["suffix"], env["arch"])
filepath = ""

if env["platform"] == "macos" or env["platform"] == "ios":
    filepath = "{}.framework/".format(env["platform"])
    file = "{}.{}.{}".format(libname, env["platform"], env["target"])

libraryfile = "bin/{}/{}{}".format(env["platform"], filepath, file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

install_file_name = "lib{}.{}.{}{}".format(libname, env["platform"], env["target"], env["suffix"])
copy = env.InstallAs("{}/bin/{}/{}{}".format(projectdir, env["platform"], filepath, install_file_name), library)

default_args = [library, copy]
Default(*default_args)
