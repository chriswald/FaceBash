import mechanize as mech
import sys

# Instantiate a Browser
browser = mech.Browser()

# Build the URI I need
uri =  'https://www.facebook.com/dialog/oauth?'
uri += 'client_id=103806206443210'
uri += '&redirect_uri=http://chriswald.com/facebash.htm'
uri += '&scope=user_about_me'
uri += '&response_type=token'

# Pretend I'm not a bot
browser.set_handle_robots(False)
cookies = mech.CookieJar()
browser.set_cookiejar(cookies)
browser.addheaders = [('User-agent', 'Mozilla/5.0 (X11; U; Linux i686; en-US) AppleWebKit/534.7 (KHTML, like Gecko) Chrome/7.0.517.41 Safari/534.7')]

# This is where the magic happens
# Open the URI, fill out the forms and submit everything
browser.open(uri)
browser.select_form(nr=0)
browser['email'] = sys.argv[1]
browser['pass'] = sys.argv[2]
response1 = browser.submit(name='login', label='Log In')

return_val = 'fail'

# If we were redirected to a facebook site ask the user for permission to
# add this application to their trusted list.
if 'facebook' in response1.geturl():
    sys.stdout.write('Do you want to grant this app permissions to your Facebook account? (y or n): ')
    answer = str(raw_input())
    if answer == 'Y' or answer == 'y':
        browser.select_form(nr=2)
        response1 = browser.submit(nr=0)
        return_val = response1.geturl()
else:
    return_val = response1.geturl()

print return_val
