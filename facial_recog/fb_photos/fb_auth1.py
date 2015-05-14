import facebook
import urllib
import urlparse
import subprocess
import warnings

warnings.filterwarnings('ignore', category=DeprecationWarning)

FACEBOOK_APP_ID = '405859292931281'
FACEBOOK_APP_SECRET = ''
FACEBOOK_PROFILE_ID = '100003271211216'
#'ziqi.peng.92'

oauth_args = dict(client_id = FACEBOOK_APP_ID, client_secret = FACEBOOK_APP_SECRET, grant_type = 'client_credentials')
oauth_curl_cmd = ['curl', 'https://graph.facebook.com/oauth/access_token?' + urllib.urlencode(oauth_args)]
oauth_response = subprocess.Popen(oauth_curl_cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE).communicate()[0]
try:
	oauth_access_token = urlparse.parse_qs(str(oauth_response))['access_token'][0]
	print oauth_access_token
except KeyError:
	print 'Unable to grab an access token.'
	exit()

facebook_graph = facebook.GraphAPI(oauth_access_token)
try:
	fb_conn = facebook_graph.get_connections(id=FACEBOOK_PROFILE_ID, connection_name='friends')
except facebook.GraphAPIError as e:
	print "Cannot get connections:", e.type, e.message
try:
	fb_response = facebook_graph.put_wall_post('test api yaaaa', profile_id = FACEBOOK_PROFILE_ID)
	print fb_response
except facebook.GraphAPIError as e:
	print 'Something went wrong:', e.type, e.message
