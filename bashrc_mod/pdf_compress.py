import os
import sys

cmd_head="gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/"
quality_high="prepress"
quality_mid="ebook"
quality_low="screen"
cmd_tail=" -dNOPAUSE -dQUIET -dBATCH -sOutputFile="

# "gs -sDEVICE=pdfwrite -dCompatibilityLevel=1.4 -dPDFSETTINGS=/prepress -dNOPAUSE -dQUIET -dBATCH -sOutputFile=1.pdf PAN_mummy.pdf"

try:
	ip_file=str(sys.argv[1])
except:
	print("No input file provided")
	exit()

cmd=cmd_head

try:
	compression_factor=int(sys.argv[2]) # 1,2,3
	if(compression_factor==1):
		cmd+=quality_high
	elif(compression_factor==2):
		cmd+=quality_mid
	elif(compression_factor==3):
		cmd+=quality_low
	else:
		print("Select compression from Range 1 - 3")
		exit()
except:
	print("Select compression from Range 1 - 3")
	exit()


cmd+=cmd_tail

try:
	op_file=str(sys.argv[3])
except:
	op_file="compressed_"+ip_file

cmd+=ip_file
cmd+=" "
cmd+=op_file
print("Compressing "+ip_file+" with compression_factor of "+str(compression_factor)+".Output file: "+op_file)
print(cmd)
os.system(cmd)
