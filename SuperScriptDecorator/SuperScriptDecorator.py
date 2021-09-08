import CustomConfig
import DecoratorCore
import os
import sys
import getopt
import logging

try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:e:c:fd")
except getopt.GetoptError:
    print('Wrong arguments!')
    print('python SuperScriptViewer.py -i <import.txt> -o <decorated.db> -c <codec_name> -d')
    sys.exit(1)
for opt, arg in opts:
    if opt == '-h':
        print('python SuperScriptViewer.py -i <import.txt> -o <decorated.db> -c <codec_name> -d')
        sys.exit(0)
    elif opt == '-i':
        CustomConfig.export_db = arg
    elif opt == '-o':
        CustomConfig.decorated_db = arg
    elif opt == '-c':
        CustomConfig.database_encoding = arg
    elif opt == '-d':
        CustomConfig.debug_mode = True

print('Super Script Decorator')
print('Homepage: https://github.com/yyc12345/SuperScriptMaterializer')
print('Report bug: https://github.com/yyc12345/SuperScriptMaterializer/issues')
print('')

# process input and output
if not os.path.isfile(CustomConfig.export_db):
    print('No import.txt. Fail to generate. Exit app.')
    sys.exit(1)

# real db generator func
def dc_wrapper():
    pass

# generate db
if CustomConfig.debug_mode:
    DecoratorCore.run()
else:
    try:
        DecoratorCore.run()
    except Exception, ex:
        print("!!! An error occurs. Please report follwoing error output and reproduce file to developer. !!!")
        logging.exception(ex)
        sys.exit(1)

print('Decorated database generating done.')

