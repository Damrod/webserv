import requests

# MISC
def test_method_not_implemented_501():
    url = 'http://localhost:8080/'
    response = requests.options(url)
    assert response.status_code == 501
