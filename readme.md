This is open source project on neural net.
# To build the project:
1. Download the repository from https://github.com/KrolPrzemyslaw/ELECTRO_BRAIN
2. Enter project directory with the command line and type:
```
cmake ../ELECTRO_BRAIN
cmake --build .
```
Additionally, ELECTRO_BRAIN may be installed on Linux using:
```
sudo cmake --install .
```

# To run ELECTRO_BRAIN 
If you have installed ELECTRO_BRAIN, use command line:
```
ELECTRO_BRAIN conf_file_name	- to run program with selected configuration file
ELECTRO_BRAIN -h		          - to display help, describing configuration file structure
```
If you have not installed it, only built, you have to type the following in the command line, when in projects directory:
```
./ELECTRO_BRAIN conf_file_name	- to run program with selected configuration file
./ELECTRO_BRAIN -h		          - to display help, describing configuration file structure
```
# To test ELECTRO_BRAIN 
When in projects directory use command line:
```
ctest
```
This will run tests checking single-input neurone's responses.
