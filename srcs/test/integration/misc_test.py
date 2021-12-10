import requests

# MISC
def test_method_not_implemented_501():
    url = 'http://localhost:8080/'
    response = requests.options(url)
    assert response.status_code == 501

def test_invalid_cgi_binary_500():
    url = 'http://localhost:8094/info.php'
    response = requests.get(url)
    assert response.status_code == 500

def test_cgi_redirect_301():
    url = 'http://localhost:8094/redirect.py'
    redirect_url = "https://www.google.com"
    response = requests.get(url, allow_redirects=False)
    assert response.status_code == 301
    assert response.headers['Location'] == redirect_url

def test_invalid_no_output_cgi_script_500():
    url = 'http://localhost:8094/no_output.py'
    response = requests.get(url)
    assert response.status_code == 500

def test_invalid_headers_cgi_script_500():
    url = 'http://localhost:8094/invalid_headers.py'
    response = requests.get(url, timeout=5)
    process_is_running = False
    for process in psutil.process_iter():
        arg = 'html/web6/invalid_headers.py'
        try:
            if arg in process.cmdline():
                process_is_running = True
        except:
            pass
    assert not process_is_running
    assert response.status_code == 500
