#!/usr/bin/python

import sys
import logging
from gurux_dlms import *


cipher = sys.argv[1]
data = sys.argv[2]

decrypt = GXDLMSTranslator(TranslatorOutputType.SIMPLE_XML)
decrypt.comments = True
decrypt.blockCipherKey = GXByteBuffer.hexToBytes(cipher)
msg = decrypt.messageToXml(GXByteBuffer.hexToBytes(data))

msg = msg.replace('<DataNotification>', '--> <DataNotification>')
msg = msg.replace("</DataNotification>\r\n-->", "</DataNotification>")
msg = msg.replace('L"', '"')

if msg == "":
        print ("no decoding possible")
else:
	print (msg)

