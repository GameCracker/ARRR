import sys
import urllib2
import urllib
import json
import urlparse
import inspect
from inspect import currentframe, getframeinfo

def line():
    print "line", inspect.currentframe().f_back.f_lineno

def parse_str(str):
    nvps={};
    list = str.rsplit("&")
    for el in list:
        nvplist = el.rsplit("=")
        nvps[nvplist[0]]=nvplist[1]
    return nvps
 
def getAccessTokenDetails(app_id,app_secret,redirect_url,code):
 
    list ={}
    url =  "https://graph.facebook.com/oauth/access_token?client_id="+app_id+"&redirect_uri="+redirect_url+"&client_secret="+app_secret+"&code="+code;
    req = urllib2.Request(url)
    try: 
        response= urllib2.urlopen(req)
        str=response.read()
        #you can replace it with urlparse.parse_qs
        list = parse_str(str)
 
    except Exception, e:
        line()
        print e
    return list
 
def getUserDetails(access_token):
    list={}
    url = "https://graph.facebook.com/me?access_token="+access_token;
    req = urllib2.Request(url)
    try: 
        response= urllib2.urlopen(req)
        str=response.read()
        list = json.dumps(str)
    except Exception, e:
        line()
        print e
 
    return list
###########
 
APP_ID = '405859292931281'
APP_SECRET = '95530311a1cd1386bfd929b9f163e0b9'
REDIRECT_URL = 'http://localhost:8000/registration/'
CODE=''  ###HTTP GET Parameter received from Facebook 
 
details = getAccessTokenDetails(APP_ID,APP_SECRET,REDIRECT_URL,CODE)
print details
 
if not details["access_token"] is None:
    jsonText =getUserDetails(details["access_token"])
 
    #######Load it twice... Need to check
    jsonText =json.loads(jsonText)
    userInfo =json.loads(jsonText)
    #######Load it twice... Need to check
 
    print "Name: "+ userInfo['name']
    print "id: "+ userInfo['id']
    print "link: "+ userInfo["link"]