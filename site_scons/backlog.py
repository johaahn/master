import os, sys, math
from termcolor import colored
from bs4 import BeautifulSoup
import re

def ParagraphLine(line,styles):
    from reportlab.platypus import Paragraph
    if line:
        line = re.sub(' +', ' ', line)
        line = line.replace('\n','<br/>')
    else:
        line = ' '
    return Paragraph(line, styles)

def chopLine(line, maxline):
    line = re.sub(' +', ' ', line)
    line = line.replace('\n','<br>')
    #line = line.replace('\n','').strip()
    line = line.strip()
    cant = int(len(line) / maxline)
    cant += 1
    strline = ""
    index = maxline
    for i in range(1,cant):
        index = maxline * i
        strline += "%s\n" %(line[(index-maxline):index])
    strline += "%s\n" %(line[index:])
    return strline

class backlog:
    def __init__(self,project, filter_version):
        self.dummy = 0
        self.specs = {}
        self.specList = []
        self.nb_of_tests = 0
        self.nb_of_tests_ok = 0
        self.nb_of_tasks = 0
        self.nb_of_tasks_ok = 0
        self.project = project
        self.filter_version = filter_version
        self.version = None

    def append(self, file):
        with open(file, 'r') as content_file:
            content = content_file.read()
            soup = BeautifulSoup(content,'lxml')
            for project in soup.find_all('project'):
                    for version in project.find_all('version'):
                        self.version = version.string
                    for name in project.find_all('name'):
                        self.project = name.string
            for spec in soup.find_all('spec'):
                nb_of_tasks_ok = 0
                nb_of_tasks = 0
                info = {}
                info['uid'] = spec.uid.string
                info['requirement'] = spec.exigence.string
                for description in spec.find_all('description'):
                    info['description'] = description.string

                info['tasks'] = []
                for task in spec.find_all('task'):
                    task_info = {}

                    for description in task.find_all('description'):
                        task_info['description']=description.string

                    for who in task.find_all('who'):
                        task_info['who']=who.string

                    for status in task.find_all('status'):
                        task_info['status']=status.string
                        if status.string == "OK":
                            nb_of_tasks_ok += 1
                    nb_of_tasks += 1
                    info['tasks'].append(task_info)


                bypass_spec = False
                if self.filter_version:
                    bypass_spec = True

                info['tests'] = []
                for test in spec.find_all('test'):
                    test_info = {}
                    for iadt in test.find_all('iadt'):
                        test_info['iadt']=iadt.string
                    for what in test.find_all('what'):
                        test_info['what']=what.string
                    for who in test.find_all('who'):
                        test_info['who']=who.string
                    for when in test.find_all('when'):
                        test_info['when']=when.string
                    for ref in test.find_all('ref'):
                        test_info['ref']=ref.string
                    for result in test.find_all('result'):
                        test_info['result']=result.string
                    for version in test.find_all('version'):
                        test_info['version']=version.string


                    bypass_test = False
                    if self.filter_version:
                        bypass_test = True
                        if 'version' in test_info:
                            if re.match(self.filter_version, test_info['version']):
                                bypass_test = False
                                bypass_spec = False

                    if not bypass_test:
                        if 'result' in test_info:
                            if test_info['result'] == "OK":
                                self.nb_of_tests_ok += 1
                        self.nb_of_tests += 1
                        info['tests'].append(test_info)

                # Append info to spec list
                if not bypass_spec:
                    self.nb_of_tasks_ok += nb_of_tasks_ok
                    self.nb_of_tasks += nb_of_tasks
                    if info['uid'] in self.specs:
                            print(colored("ERROR ",'red')+ "ID "+info['uid']+" redifined")
                            sys.exit(-1)
                    self.specs[info['uid']] = info
                    self.specList.append(info)

    def display(self):
        import time
        from reportlab.lib import colors
        from reportlab.lib.enums import TA_JUSTIFY, TA_CENTER
        from reportlab.lib.pagesizes import A3, landscape
        from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image, Table, TableStyle
        from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
        from reportlab.lib.units import inch, mm

        doc = SimpleDocTemplate("form_letter.pdf",pagesize=landscape(A3),
                                rightMargin=36,leftMargin=36,
                                topMargin=18,bottomMargin=18)
        Story=[]
        formatted_time = time.ctime()

        #im = Image(logo, 2*inch, 2*inch)
        #Story.append(im)

        styles=getSampleStyleSheet()
        styles.add(ParagraphStyle(name='Justify', alignment=TA_JUSTIFY))
        styles.add(ParagraphStyle(name='TitleT', alignment=TA_CENTER ,textColor=colors.white))
        ptext = '<font size=24>BACKLOG:%s</font>' % self.project
        Story.append(Paragraph(ptext, styles["Normal"]))
        Story.append(Spacer(1, 24))
        ptext = '<font size=12>%s</font>' % formatted_time
        Story.append(Paragraph(ptext, styles["Normal"]))
        Story.append(Spacer(1, 12))
        r_tests = int(100.0*self.nb_of_tests_ok/self.nb_of_tests)
        r_tasks = int(100.0*self.nb_of_tasks_ok/self.nb_of_tasks)
        ptext = '<font size=12>%d %% Tests passed, %d %% Tasks done </font>' % (r_tests,r_tasks)
        Story.append(Paragraph(ptext, styles["Normal"]))
        Story.append(Spacer(1, 12))
        if self.filter_version:
            ptext = '<font size=12>Version filter: %s </font>' % (self.filter_version)
            Story.append(Paragraph(ptext, styles["Normal"]))
            Story.append(Spacer(1, 12))
        if self.version:
            ptext = '<font size=12>Version: %s </font>' % (self.version)
            Story.append(Paragraph(ptext, styles["Normal"]))
            Story.append(Spacer(1, 12))

        # Create return address

        col = {}
        col['uid'] = 0
        col['requirement'] = 1
        col['task'] = 2
        col['task_who'] = 3
        col['task_status'] = 4
        col['iadt'] = 5
        col['what'] = 6
        col['who'] = 7
        col['when'] = 8
        col['result'] = 9
        col['version'] =10
        col['ref'] = 11
        colInfoStart = 0
        colInfoEnd = col['requirement']
        colTaskStart = col['task']
        colTaskEnd = col['task_status']
        colTestStart = col['iadt']
        colTestEnd = col['ref']

        data= []
        dataStyle= [
                    # Format Titles
                    ('ALIGN',(0,0),(-1,1),'CENTER'),
                    ('SPAN', (0,0), (colInfoEnd,0)),
                    ('SPAN', (colTaskStart,0), (colTaskEnd,0)),
                    ('SPAN', (colTestStart,0), (-1,0)),
                    ('BACKGROUND',(0,0),(-1,1), colors.navy),
                    ('TEXTCOLOR',(0,0),(-1,1), colors.white),

                    # Align UID
                    ('ALIGN',(0,0),(0,-1),'CENTER'),
                    ('BACKGROUND',(0,2),(0,-1),colors.silver),

                    # Aling test result
                    ('ALIGN',(-4,2),(-1,-1),'CENTER'),

                    # Set Test as Failed by Default
                    ('BACKGROUND',(col['task_status'],2),(col['task_status'],-1), colors.firebrick),
                    ('BACKGROUND',(col['result'],2),(col['result'],-1), colors.firebrick),

                    # Global
                    ('VALIGN',(0,0),(-1,-1),'MIDDLE'),
                    ('INNERGRID', (0,0), (-1,-1), 0.25, colors.black),
                    ('BOX', (0,0), (-1,-1), 0.25, colors.black),
                    ]

        data.append([Paragraph('<b>Fonctionnalites</b>',styles["TitleT"]),
            '',
            Paragraph('<b>Travaux</b>',styles["TitleT"]),
            '',
            '',
            Paragraph('<b>Verification</b>',styles["TitleT"]),
            '',
            '',
            '',
            '',
            ''
            ])

        data.append([
            Paragraph('<b>UID</b>',styles["TitleT"]),
            Paragraph('<b>Exigences</b>',styles["TitleT"]),
            Paragraph('<b>Travaux</b>',styles["TitleT"]),
            Paragraph('<b>Qui</b>',styles["TitleT"]),
            Paragraph('<b>Status</b>',styles["TitleT"]),
            Paragraph('<b>IADT</b>',styles["TitleT"]),
            Paragraph('<b>Quoi</b>',styles["TitleT"]),
            Paragraph('<b>Qui</b>',styles["TitleT"]),
            Paragraph('<b>Quand</b>',styles["TitleT"]),
            Paragraph('<b>OK</b>',styles["TitleT"]),
            Paragraph('<b>Version</b>',styles["TitleT"]),
            Paragraph('<b>Preuves</b>',styles["TitleT"]),
            ])
        colWidths = (
            25*mm,
            70*mm,

            70*mm,
            20*mm,
            10*mm,

            10*mm,
            100*mm,
            20*mm,
            20*mm,
            10*mm,
            30*mm,
            30*mm
            )


        for spec in self.specList:
            idrow = len(data)
            nbrow = max(max(len(spec['tests']), len(spec['tasks'])),1)
            if nbrow != 1:
                print(idrow,nbrow)
                dataStyle.append(('SPAN', (0,idrow), (0,idrow+nbrow-1)))
                dataStyle.append(('SPAN', (1,idrow), (1,idrow+nbrow-1)))
            dataStyle.append(('BOX',  (0,idrow), (-1,idrow+nbrow-1), 2, colors.black))

            for id in range(nbrow):
                row = []
                idrow = len(data)
                row.append(spec['uid'])
                row.append(ParagraphLine(spec['requirement'],styles["BodyText"]))

                id_task = id
                if id_task < len(spec['tasks']):
                    if 'description' in spec['tasks'][id_task]:
                        desc =spec['tasks'][id_task]['description']
                        #desc = chopLine(desc, 30)
                        row.append(ParagraphLine(desc,styles["BodyText"]))
                    else:
                        row.append('N/A')

                    if 'who' in spec['tasks'][id_task]:
                        row.append(spec['tasks'][id_task]['who'])
                    else:
                        row.append('N/A')

                    if 'status' in spec['tasks'][id_task]:
                        status = spec['tasks'][id_task]['status']
                        row.append(status)
                        if status:
                            if status.upper() == 'OK':
                                dataStyle.append(('BACKGROUND',(col['task_status'],idrow), (col['task_status'],idrow), colors.seagreen))
                            if '%' in status:
                                    dataStyle.append(('BACKGROUND',(col['task_status'],idrow), (col['task_status'],idrow), colors.darkorange))
                    else:
                        row.append('N/A')

                else:
                    row.append('')
                    row.append('')
                    row.append('')
                    dataStyle.append(('SPAN', (colTaskStart,idrow), (colTaskEnd,idrow)))
                    dataStyle.append(('BACKGROUND',(colTaskStart,idrow), (colTaskEnd,idrow), colors.tan))

                id_test = id
                if id_test < len(spec['tests']):
                    if 'iadt' in spec['tests'][id_test]:
                        iadt = (spec['tests'][id_test]['iadt']).upper()
                        row.append(iadt)
                        if iadt == 'I':
                          dataStyle.append(('BACKGROUND',(col['iadt'],idrow), (col['what'],idrow), colors.paleturquoise))
                        elif iadt == 'A':
                          dataStyle.append(('BACKGROUND',(col['iadt'],idrow), (col['what'],idrow), colors.seagreen))
                        elif iadt == 'D':
                          dataStyle.append(('BACKGROUND',(col['iadt'],idrow), (col['what'],idrow), colors.sandybrown))
                        elif iadt == 'T':
                          dataStyle.append(('BACKGROUND',(col['iadt'],idrow), (col['what'],idrow), colors.salmon))
                    else:
                        row.append('N/A')

                    if 'what' in spec['tests'][id_test]:
                        what = spec['tests'][id_test]['what']
                        row.append(ParagraphLine(what,styles["BodyText"]))
                    else:
                        row.append('N/A')

                    if 'who' in spec['tests'][id_test]:
                        row.append(ParagraphLine(spec['tests'][id_test]['who'],styles["BodyText"]))
                    else:
                        row.append('N/A')

                    if 'when' in spec['tests'][id_test]:
                        row.append(spec['tests'][id_test]['when'])
                    else:
                        row.append('N/A')

                    if 'result' in spec['tests'][id_test]:
                        result = spec['tests'][id_test]['result']
                        row.append(result)
                        if result == "OK":
                            dataStyle.append(('BACKGROUND',(col['result'],idrow), (col['result'],idrow), colors.seagreen))
                    else:
                        row.append('KO')

                    if 'version' in spec['tests'][id_test]:
                        row.append(spec['tests'][id_test]['version'])
                    else:
                        row.append('N/A')

                    if 'ref' in spec['tests'][id_test]:
                        row.append(chopLine(spec['tests'][id_test]['ref'],11))
                    else:
                        row.append('N/A')
                else:
                    row.append('')
                    row.append('')
                    row.append('')
                    row.append('')
                    row.append('')
                    row.append('')
                    row.append('')
                    dataStyle.append(('SPAN', (colTestStart,idrow), (colTestEnd,idrow)))
                    dataStyle.append(('BACKGROUND',(colTestStart,idrow), (colTestEnd,idrow), colors.tan)),
                    #if id_test == nbrow-1:
                    #    nb_offset = nbrow - len(spec['tests'])
                    #dataStyle.append(('SPAN', (colTestStart,idrow), (colTestEnd,idrow)))
                    #    for colt in range(colTestStart, colTestEnd):
                    #        dataStyle.append(('SPAN', (colt,idrow-nb_offset), (colt,idrow)))

                data.append(row)

        t=Table(data, colWidths=colWidths)
        t.setStyle(TableStyle(dataStyle))

        Story.append(t)
        doc.build(Story)


if __name__ == "__main__":
    # execute only if run as a script
    import argparse
    parser = argparse.ArgumentParser(description='Parse backlog files.')
    parser.add_argument('files',
                        metavar='FILES',
                        nargs='+',
                        help='files to open')
    parser.add_argument("--filter", help="version filter")
    parser.add_argument('--debug')
    args = parser.parse_args()
    print(args.files)

    if not isinstance(args.files, list):
        files = [args.files]
    else:
        files = args.files

    filter_version = args.filter
    print(filter_version)
    #filter_version = '0\.\d-?.*'
    bl = backlog(os.path.basename(files[0]), filter_version)
    for file in files:
        bl.append(file)
    bl.display()
