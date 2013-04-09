#/usr/bin/python
import os,re,sys
import Arg_split
#import ASCII_Color

codeCodes = {
    'black':    '0;30',     'bright gray':  '0;37',
    'blue':     '0;34',     'white':        '1;37',
    'green':    '0;32',     'bright blue':  '1;34',
    'cyan':     '0;36',     'bright green': '1;32',
    'red':      '0;31',     'bright cyan':  '1;36',
    'purple':   '0;35',     'bright red':   '1;31',
    'yellow':   '0;33',     'bright purple':'1;35',
    'dark gray':'1;30',     'bright yellow':'1;33',
    'normal':   '0'
}
 
def printc(text, color):
    """Print in color."""
    print "\033["+codeCodes[color]+"m"+text+"\033[0m"
 
def writec(text, color):
    """Write to stdout in color."""
    sys.stdout.write("\033["+codeCodes[color]+"m"+text+"\033[0m")
 
def switchColor(color):
    """Switch console color."""
    sys.stdout.write("\033["+codeCodes[color]+"m")

def paintc(text,color):
    return "\033["+codeCodes[color]+"m"+text+"\033[0m"
     
#function. show file name show the line
def _list_needed_file(opt,args):
    _prop_list = opt.prop_file.split(" ")
    keyword_color = "cyan"
    file_cnt = 0
    keyword_total_cnt = 0
    for(_path, _dirs, _files) in os.walk(opt.dir_entry):
        for _file_name in _files:
            if(opt.prop_file != "-1" and  not (_file_name.count(".") and _file_name[_file_name.rindex("."):] in _prop_list)):
                continue
            try:
                fd = open(_path+ os.sep + _file_name,"rb")
            except IOError:
                continue
            fd_content = fd.readlines()
            _1st_found = True
            for (index, line) in enumerate(fd_content):
                keyword_cnt_each_line = line.count(opt.keyword)
                if(keyword_cnt_each_line <=  0):
                    continue

                if(_1st_found == True):
                    file_cnt = file_cnt + 1;
                    print "\n","["+ str(file_cnt) +"]"+_path + os.sep + _file_name
                    _1st_found = False

#                print "\t" "["+ str(keyword_cnt_each_line) +"]", index +1, line.replace(opt.keyword, paintc(opt.keyword,keyword_color)),
                if(opt.ascii):
                    print "\t" , index +1, line.replace(opt.keyword, paintc(opt.keyword,keyword_color)),
                else:
                    print "\t" , index +1, line,
                keyword_total_cnt = keyword_total_cnt + keyword_cnt_each_line
            fd.close()
    print "\nCalculation:"
    print "The number of File owning keyword: " + str(file_cnt)
    print "The keyword count: " + str(keyword_total_cnt)

    return
if __name__  == '__main__':
    splitter = Arg_split.Arg_split("./listKeywordFile.py -w keyword")
    opt, args = splitter.default_split_out()
    _list_needed_file(opt,args)
    
#To do
#repeat keyword in the line 1.count 2. chang their color : use the smart way(like re to change all.) to replace.
