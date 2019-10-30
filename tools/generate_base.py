#!/usr/bin/python

import argparse
import re
import hashlib
import json
import anydbm


def encrypt_id(hash_string):
    sha_signature = hashlib.sha256(hash_string.encode()).hexdigest()
    return int(sha_signature[0:8],16)

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('files', metavar='FILES', type=argparse.FileType('r'), nargs='+',
                    help='files to open')
args = parser.parse_args()
print(args.files)

id_list = []


for file_tmp in args.files:
  print(file_tmp)
  for line in file_tmp:
    if "E_ID" in line:
      m = re.search(r'E_ID_([A-Z0-9]+)_([A-Z0-9_]+)', line)
      if m :
	id_list.append(m.group(0))
	
id_set = list(set(id_list))
print(id_set)

db_word = {}
db_id = {}
for id_tmp in id_set:
  e_id = encrypt_id(id_tmp);
  db_word[id_tmp] = e_id
  db_id[e_id] = id_tmp


with open('/tmp/test.json', 'w') as f:
    json.dump(db_word, f)
  

db = anydbm.open('/tmp/test.dbm', 'c')

for k, v in db.iteritems():
    print k, '\t', v

for id_tmp in id_set:
  e_id = encrypt_id(id_tmp);
  e_id_str = str(e_id)
  if e_id_str not in db:
	  db[e_id_str] = id_tmp
  else:
    #TODO: Increment id until room available !!
    assert(db[e_id_str] == id_tmp)

db.close()
