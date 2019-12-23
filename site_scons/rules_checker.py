import os, sys
from pyparsing import *
from termcolor import colored

rules_list = [
    {
    'identifiers':[
        'int',
        'uint',
        ],
    'prefix':'i'
    },
    {
    'identifiers':[
        'uint8_t',
        'uint16_t',
        'uint32_t',
        'uint64_t',
        'int8_t',
        'int16_t',
        'int32_t',
        'int64_t'
        ],
    'prefix':'i'
    },
    {
    'identifiers':['size_t'],
    'prefix':'sz'
    },
    {
    'identifiers':['bool'],
    'prefix':'b'
    },
    {
    'identifiers':['char'],
    'prefix':'h'
    },
    {
    'identifiers':['string'],
    'prefix':'str'
    },
    {
    'identifiers':['vector', 'ct_shared_vector'],
    'prefix':'v'
    },
    {
    'identifiers':['map'],
    'prefix':'m'
    },
    {
    'identifiers':['list'],
    'prefix':'l'
    },
    {
    'identifiers':['float3_t', 'float2_t', 'float3', 'float2'],
    'prefix':'s'
    },
    {
    'identifiers':[
        'float',
        'double'
        ],
    'prefix':'f'
    },
]

class rules_checker:
    def __init__(self, filename, test_mode=None, debug_mode=[], include_list=[], include_path=[], rules=[]):
        self.filename = filename
        print("Rules checking: %s"%(filename))
        self.include_path = include_path
        self.basepath = include_path
        self.basepath.append(os.path.dirname(filename))
        self.subfile = include_list
        self.level = 0
        self.mode_level = 0
        self.mode = []
        self.error = 0
        self.rules = rules
        self.debug = debug_mode
        self.test_mode = test_mode
        self.last = ""
        self.class_mode = ""

        fp = open(filename,"r")
        if fp:
            self.parse_file(fp)
            fp.close()
        #print('ERR',self.error)

    def check_test(self, mode, name, error):
        test_failed = False

        if not self.test_mode:
            return

        # Name must contain test
        if name.lower().find("test") == -1:
            print("test not found")
            test_failed = True

        # Name must contain mode
        if name.lower().find(mode) == -1:
            print(mode + " not found")
            test_failed = True

        # Depending on name, we expect an error or not
        if name.lower().find('bad') != -1:
            if not error:
                print("expect bad")
                test_failed = True
        else :
            if error:
                print("expect good")
                test_failed = True

        if test_failed:
            print(colored("ERROR ",'red')+ "TEST: "+name)
            sys.exit(-1)
        else:
            print(colored("PASS ",'green')+ "TEST: "+name)

    def check_prefix(self, name, prefix_list, loc, s):
        lineno_ = lineno(loc, s)
        colno_ = col(loc, s)
        error = True
        for prefix in prefix_list:
            if name.startswith(prefix):
                error = False

        if len(self.rules):
            if 'prefix' not in self.rules:
                error = False

        if error:
            self.error += 1
            print(colored("ERROR:",'red')+ "%s should start with \"%s\" (line:%d,col:%d)"%(name, ' or '.join(prefix_list), lineno_, colno_))
            print("in file %s"%(self.filename))
        else:
            #if self.debug:
            if "valid" in self.debug:
                print(colored("VALID:",'green')+ "%s start with \"%s\""%(name, ' or '.join(prefix_list)))

        return error

    def parse_token(self, s, loc, tokens, type):
        if 'tok' in self.debug:
            print(tokens)
        lineno_ = lineno(loc, s)
        colno_ = col(loc, s)
        #print(loc,tokens, lineno_, colno_, type)
        self.tokens.append((tokens[0], type, lineno_, colno_))

    def parse_include_local(self, s, loc, tokens):
        #print("include", tokens)
        #print(type(tokens[1]).__name__)
        if isinstance(tokens[1], ParseResults):
            filename = tokens[1][0]
        else:
            filename = tokens[1]
        if filename not in self.subfile:
            self.subfile.append(filename)

            found = False
            for basepath in self.basepath:
                rootpath = os.path.join(basepath, filename)
                #print('Opening include:' +rootpath)
                if os.path.exists(rootpath):
                    found = True
                    rsub = rules_checker(rootpath, test_mode=self.test_mode, debug_mode=self.debug, include_list=self.subfile, include_path=self.include_path, rules=self.rules)
                    self.error += rsub.error
                    break

            if not found:
                lineno_ = lineno(loc, s)
                colno_ = col(loc, s)
                print(colored("ERROR ",'red')+ rootpath + ' does not exist (line:%d,col:%d)'%(lineno_, colno_))
                print("in file %s"%(self.filename))

    def parse_include_global(self, s, loc, tokens):
        return
    def parse_func_arg(self, s, loc, tokens):
        if "func_arg" in self.debug:
            print("FUNC ARG",tokens, self.mode)

    def check_case(self, value, up, loc, s):
        error = False
        if (value != value.lower()) and not up:
            error = True
        if (value == value.lower()) and up:
            error = True

        if len(self.rules):
            if 'case' not in self.rules:
                error = False

        if error:
            self.error += 1
            lineno_ = lineno(loc, s)
            colno_ = col(loc, s)
            if not up:
                print(colored("ERROR:",'red')+ "%s should be composed of lower cases (line:%d,col:%d)"%(value, lineno_, colno_))
            else:
                print(colored("ERROR:",'red')+ "%s should be composed of upper cases (line:%d,col:%d)"%(value, lineno_, colno_))
            print("in file %s"%(self.filename))
        return error

    def parse_func(self, s, loc, tokens):
        self.parse_token(s, loc, tokens, "FUNC")

        if "func" in self.debug:
            print("FUNC",tokens, self.mode)
        #self.mode.append(("func",self.level))
        self.last = "FUNC"
        func_name = tokens[0]

        # Ignore function that starts with operator
        if func_name.startswith('operator'):
            return;

        if self.check_case(func_name, False, loc, s):
            error = True
        elif len(self.mode) and self.mode[-1] == 'CLASS':
            #ignore constructor name
            if self.class_name.upper() == func_name:
                return;
            error = self.check_prefix(func_name,["m_", "f_"], loc, s)
        else:
            error = self.check_prefix(func_name,["f_", "__f_", "M_"], loc, s)
        self.check_test("func", func_name, error)

        list_var = []
        for token_ in tokens[1:]:
            if token_ in ['(']:
                list_var = []
            elif token_ in [')',',']:
                if "subvar" in self.debug:
                    print("SUBVAR", list_var)
                if not(len(list_var) == 1 and list_var[0] == "void"):
                    self.parse_var( s, loc, list_var, True)

                list_var = []
            else:
                list_var.append(token_)

        #self.check_prefix(var_name, type_prefix+type_suffix+'_')


    def check_ref_used(self, str_suffix, lineno_):
        
        if (str_suffix[0] == 'c') or ((str_suffix[0] == 's') and (not (str_suffix[1] == 'z'))) or (str_suffix[0] == 'l') or (str_suffix[0] == 'v') or (str_suffix[0] == 'm'):
            print(colored("WARNING ",'yellow') + " reference should be used (file:%s, line:%d, prefix:%s)"%(self.filename, lineno_, str_suffix))


    def parse_var(self, s, loc, tokens, func_mode=False):
        if "var" in self.debug:
            print("VAR",tokens, self.mode, self.level, func_mode)
        if len(tokens):
            #print("tok",tokens)
            type_suffix_list = []
            index = 0
            for tok_ in tokens:
                if str(tok_) == "[":
                    type_suffix_list.insert(0,'a')
                    break
                index = index + 1
            #print(index)
            var_name = tokens[(index-1)]
            type_list = tokens[:(index-1)]
            #print(var_name)
            if "var" in self.debug:
                print(var_name)
                print(type_list)
            type_suffix = ''
            type_prefix = ''
            type_const = False
            type_external = False
            #print(type_list)
            for typet_ in type_list:
                type_ = str(typet_).lower()
                b_found = False
                for rule in rules_list:
                    if type_ in rule['identifiers']:
                        type_suffix = rule['prefix']
                        b_found = True

                if b_found:
                    # Nothing to do
                    b_found = False
                elif type_ == '*':
                    type_suffix = 'p' + type_suffix
                    type_external = True
                elif type_ == '&':
                    type_prefix += 'r'
                    type_external = True
                elif type_ == ("CT_GUARD").lower():
                    type_suffix = 'p'
                    type_external = True
                elif type_ == ("CT_PORT_NODE_GUARD").lower():
                    type_suffix = 'pc'
                elif type_ == 'const':
                    type_const = True
                elif type_ == 'struct' or type_.startswith("st_"):
                    type_suffix = 's'
                elif type_ == 'class' or type_.startswith("ct_"):
                    type_suffix = 'c'
                elif type_.startswith("et_"):
                    type_suffix = 'e'
                elif type_ == 'auto':
                    return
                elif type_ == '<' :
                    if type_suffix in ['p','a','v','m','l']:
                        type_suffix_list.append(type_suffix)
                        type_const = False
                        type_external = False
                    else:
                        break
                elif type_ == ',' :
                    break

                if (type_suffix == 'ph' and type_const == True) or (type_external == True and type_suffix == 'h' and type_const == True):
                    type_suffix = 'str'

            if var_name == 'ec':
                return;

            if not len(self.mode) and not func_mode:
                type_prefix = 'g'+type_prefix

            #print(type_first_suffix)
            if len(type_suffix_list) :
                type_suffix = ''.join(type_suffix_list) + type_suffix
            else:
                type_suffix = type_suffix

            # Class and char use same suffix
            type_suffix = type_suffix.replace('h','c')

            if not type_suffix:
                type_suffix = ['c','s']
            else:
                type_suffix = [type_suffix]

            prefix_list = []
            for suffix in type_suffix:
                if func_mode:
                    if type_external:
                        if type_const:
                            prefix_list.append("in_"+type_prefix+suffix+'_')
                        else:
                            prefix_list.append("inout_"+type_prefix+suffix+'_')
                            prefix_list.append("out_"+type_prefix+suffix+'_')
                                                
                    else:
                        prefix_list.append("in_"+type_prefix+suffix+'_')
                        
                        self.check_ref_used(type_prefix+suffix+'_', lineno(loc, s))
                                                
                else:
                    if self.mode[-1] == "CLASS" and self.class_mode == "private":
                        prefix_list.append('_'+type_prefix+suffix+'_')
                    else:
                        prefix_list.append(type_prefix+suffix+'_')

            if self.check_case(var_name, False, loc, s):
                error = True
            else:
                error = self.check_prefix(var_name, prefix_list, loc, s)

            if error:
                print('Type:'+' '.join(type_list))
                print(type_list)
                #print(self.mode[-1],self.class_mode)
            self.check_test("var", var_name, error)

    def parse_lbra(self, s, loc, tokens):
        if "tok" in self.debug:
            print("{",tokens)
        #self.level += 1
        self.mode.append(self.last)

    def parse_rbra(self, s, loc, tokens):
        if "tok" in self.debug:
            print("}",tokens)
        #self.level -= 1
        #if len(self.mode):
        #    if self.mode[-1][1] == self.level:
        self.mode.pop()
        self.last = ""

    def parse_lpar(self, s, loc, tokens):
        #if self.debug > 1:
        lineno_ = lineno(loc, s)
        colno_ = col(loc, s)
        if "tok" in self.debug:
            print("(",tokens, lineno_, colno_)
        self.mode.append("FUNC_VAR")

    def parse_rpar(self, s, loc, tokens):
        if "tok" in self.debug:
            print(")",tokens)
        self.mode.pop()

    def parse_semicolon(self, s, loc, tokens):
        if "tok" in self.debug:
            print(";",tokens)

    def parse_class(self, s, loc, tokens):
        if "tok" in self.debug:
            print("class",tokens)
        #print("!!!!!!!!!!!11 ",tokens[0].lower())
        #self.mode.append(("class",self.level))

        type_str = str(tokens[0]).lower()
        type_name = str(tokens[1])

        if tokens[0].lower() == "class":
            self.last = "CLASS"
            self.class_mode = "private"
            self.class_name = type_name
        else:
            self.last = "STRUCT"
            self.class_mode = "public"
            self.class_name = type_name

        if self.check_case(type_name, True, loc, s):
            error = True
        elif  type_str == "struct":
            error = self.check_prefix(type_name, ["ST_"], loc,s)
        else:
            error = self.check_prefix(type_name, ["CT_"], loc,s)
        self.check_test("class", type_name, error)

    def parse_class_public(self, s, loc, tokens):
        if "class_mode" in self.debug:
            print("public",tokens)
        self.class_mode = "public"

    def parse_class_private(self, s, loc, tokens):
        if "class_mode" in self.debug:
            print("private",tokens)
        self.class_mode = "private"

    def parse_fail1(self,s,loc,expr,err):
        #if self.debug:
        print("FAILED1", err)
    def parse_fail2(self,s,loc,expr,err):
        #if self.debug:
        print("FAILED2", err)
    def parse_cpp_block(self, s, loc, tokens):
        if "tok" in self.debug:
            print(tokens)

    def parse_file(self, file):
        self.tokens = []
        # Read the input data
        TEXT     = file.read()

        # Check for bypass keyword
        if(TEXT.find('____FIXME_BYPASS_RULES____') != -1):
            print("Found ____FIXME_BYPASS_RULES____, Bypass ...")
            return

        #print(TEXT)

        # Define a simple grammar for the text, multiply the first col by 2
        identifier = Word(alphas+"_", alphas+nums+"_").setParseAction(lambda s,loc,tokens: self.parse_token(s, loc, tokens, 'identifier'))
        point = Literal( "." )
        fnumber = Combine( Word( "+-"+nums, nums ) +
                       Optional( point + Optional( Word( nums ) ) ))
        newline = White("\n")
        string = QuotedString('"',escChar='\\')
        lpar  = Literal( "(" )
        rpar  = Literal( ")" )#.setParseAction(lambda s,loc,tokens: self.parse_token(s, loc, tokens, 'rpar'))
        lcro  = Literal( "[" )
        rcro  = Literal( "]" )
        lbra  = Literal( "{" ).setParseAction(self.parse_lbra)
        rbra  = Literal( "}" ).setParseAction(self.parse_rbra)
        pp_if  = Literal( "#if" )
        pp_ifdef  = Literal( "#ifdef" )
        pp_else  = Literal( "#else" )
        pp_endif  = Literal( "#endif" )
        pp_include  = Literal( "#include" )
        pp_unknown  = Literal( "#" )
        struct_ = Literal("struct")
        class_ = Literal("class")
        class_public = Literal("public") + Literal(":").setParseAction(self.parse_class_public)
        class_private = Literal("private") + Literal(":").setParseAction(self.parse_class_private)
        class_protected = Literal("protected") + Literal(":").setParseAction(self.parse_class_private)
        semicolon  = Literal( ";" ).setParseAction(self.parse_semicolon)
        op_inf  = Literal( "<" )
        op_sup  = Literal( ">" )
        op_eg  = Literal( "=" )
        op_mul  = Literal( "*" )
        op_div  = Literal( "/" )
        op_xor  = Literal( "^" )
        op_plus  = Literal( "+" )
        op_minus  = Literal( "-" )
        op_deref  = Literal( "->" )
        op_point  = Literal( "." )
        op_and  = Literal( "&" )
        op_or  = Literal( "|" )
        op_neg  = Literal( "~" )
        op_namebase  = Literal( "::" ).setParseAction(lambda s,loc,tokens: self.parse_token(s, loc, tokens, 'ns'))
        class_init  = Literal( ":" )
        class_init_sep  = Literal( "," ).setParseAction(lambda s,loc,tokens: self.parse_token(s, loc, tokens, 'comma'))
        op_logic_neg  = Literal( "!" )
        op_logic_and  = Literal( "&&" )
        op_logic_or  = Literal( "||" )
        op_logic_eq  = Literal( "==" )
        op_logic_neq  = Literal( "!=" )
        op_logic_sl  = Literal( "<<" )#.setParseAction(lambda s,loc,tokens: self.parse_token(s, loc, tokens, 'sl'))
        op_logic_sr  = Literal( ">>" )
        comment_line = Literal( "//" )
        comment_multiline_start = Literal( "/*" )
        comment_multiline_end = Literal( "*/" )

        pp_list = pp_if | pp_ifdef | pp_endif | pp_else | pp_unknown | pp_include
        op_list = op_namebase | op_neg | op_inf | op_sup | op_eg | op_mul | op_xor | op_div | op_plus | op_minus | op_deref | op_point | op_and | op_or
        op_logic_list = op_logic_neg | op_logic_and | op_logic_or | op_logic_eq | op_logic_neq | op_logic_sl | op_logic_sr
        #block_list = lcro | rcro
        block_list = lcro | rcro  | lpar | rpar
        class_list = class_init | class_init_sep
        elements = string| identifier | block_list | semicolon | pp_list | op_logic_list | op_list | fnumber | class_list | newline
        #elements = block_list | semicolon | pp_list | op_list | fnumber | newline

        grammar = Forward()
        #bracket_block = (lbra + ZeroOrMore(grammar) + rbra).setParseAction(self.parse_token_bra)
        #par_block = (lpar + ZeroOrMore(grammar) + rpar).setParseAction(self.parse_token_par)
        comment_block_ml = nestedExpr("/*","*/")
        comment_block_if0 = nestedExpr("#if 0","#endif")
        comment_block_sl = comment_line + SkipTo(newline)
        include_block_global = (pp_include + (nestedExpr("<",">"))).setParseAction(self.parse_include_global)
        include_block_local = (pp_include + (QuotedString('"'))).setParseAction(self.parse_include_local)
        cpp_block = elements.setParseAction(self.parse_cpp_block)

        type = Forward()
        type << (
            Optional(Literal("const"))
            + Optional(Literal("struct")|Literal("class"))
            + Optional(OneOrMore(identifier+op_namebase)).suppress()
            + identifier
            + Optional(Literal("<") + OneOrMore(type) + Literal(">"))
            + Optional(Literal("const"))
            + Optional(Literal("&")|(OneOrMore(Literal("*")+Optional(Literal("const")).suppress())))
        )

        operator_identifier = (
          Literal( "operator==" )
          | Literal( "operator+=" )
          | Literal( "operator-=" )
          | Literal( "operator!=" )
        )

        variable = (
            type
            + identifier
        )

        variable_block = (
            variable
            + Optional(Optional(Literal("[") + (identifier|Word(nums)) + Literal("]"))|nestedExpr("(",")").suppress())
            + (Literal("=")|semicolon).suppress()
            ).setParseAction(self.parse_var)

        return_block = (
            Literal("return")
            + identifier
        )
        delete_block = (
            Literal("delete")
            + identifier
        )
        goto_block = (
            Literal("goto")
            + identifier
        )
        class_block = (
            (
                (class_|struct_)
                + Optional(OneOrMore(identifier+op_namebase)).suppress()
                + identifier
                ).setParseAction(self.parse_class)
                #+ lbra
        )#.setFailAction(self.parse_fail1)
        function_block = (
                (
                    Optional(Literal("template")+nestedExpr("<",">")).suppress()
                    + Optional(Literal("virtual")).suppress()
                    + Optional(Literal("static")).suppress()
                    + Optional(type).suppress()
                    + Optional(identifier+Literal( "::" )).suppress()
                    + (operator_identifier | identifier)
                    + (
                    lpar#.setParseAction(self.parse_lpar)
                    + Optional(Literal("void") | OneOrMore(variable + Optional(Literal(","))))
                    + rpar#.setParseAction(self.parse_rpar)
                    )#.setParseAction(self.parse_func_arg)
                ).setParseAction(self.parse_func)#.setFailAction(self.parse_fail1)
            #+ (semicolon|lbra).suppress()
            )#.setFailAction(self.parse_fail2)
        expr = (include_block_local |
            include_block_global |
            comment_block_ml |
            comment_block_sl |
            comment_block_if0 |
            (lbra | rbra) |
            goto_block |
            delete_block |
            return_block |
            (newline + class_block) |
            (newline + StringEnd()) |
            (newline + function_block) |
            (newline + variable_block) |
            cpp_block #|
            #StringEnd()
            )# |bracket_block | par_block
        #blocks   =  cpp_block | newline
        grammar  << OneOrMore(expr)

        # Parse the results
        result = grammar.parseString( TEXT )
        # This gives a list of lists
        # [['cat', 6], ['dog', 10], ['foo', 14]]

        #print("End of rules checking: %s"%(self.filename))

        return
        #print self.tokens

        self.heap_mode = []
        self.level = 0
        for value, type, line_, col_ in self.tokens:
            print(value, type)
            if type == "identifier":
                if value == "class":
                    self.heap_mode.append(("class", self.level))
                elif value == "struct":
                    self.heap_mode.append(("class", self.level))
            elif type == "semicolon":
                if len(self.heap_mode):
                    if(self.level == self.heap_mode[-1][1]):
                        self.heap_mode.pop()

        #    print(token)

if __name__ == "__main__":
    # execute only if run as a script
    import argparse
    parser = argparse.ArgumentParser(description='Parse CPP files.')
    parser.add_argument('files',
                        metavar='FILES',
                        nargs='+',
                        help='files to open')
    parser.add_argument('--test', action='store_true')
    parser.add_argument('--debug')
    parser.add_argument('-I', action='append')
    args = parser.parse_args()
    print(args.files)
    print(args.test)
    print(args.I)

    if not isinstance(args.files, list):
        files = [args.files]
    else:
        files = args.files

    if not args.I:
        args.I = []

    debug = []
    if args.debug:
        debug = args.debug.split(',')

    for file in files:
        r = rules_checker(file, test_mode=args.test, debug_mode=debug , include_list=[], include_path=args.I)
