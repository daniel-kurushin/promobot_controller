from bs4 import BeautifulSoup as BS

soup = BS(open('out').read())
print(soup.text)