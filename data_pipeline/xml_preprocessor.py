from bs4 import BeautifulSoup
import re
#this script remove the repetition at the end of each chapter and remove comments

def xml_preprocessor(inFilename, outFilename, removedTextFilename) :
    with open(inFilename,"r") as data :
        soup = BeautifulSoup(data, "xml")

    books = soup.find_all("div", type="chapter")

    #remove repeating text at the end of each chapter
    for book in books :
        chapters = list(book.find_all("seg"))
        
        el1 = chapters[-2] #before the last
        el2 = chapters[-1] # the last element
        
        t1 = el1.getText()
        t2 = el2.getText().expandtabs().strip() #transform tabs into spaces and remove extra spaces
        
        if t2 in t1 :
            t1 = t1[:t1.find(t2)] #remove t2 from t1
            el1.string = t1

    #remove comments from each verses
    #and keep removed text in removed
    verses = soup.find_all("seg", type="verse")


    with open(removedTextFilename, "w") as removed :
        for v in verses :
            s = re.search("^(.+?)(\[ *(Gr|Heb|Na|Ampio hoe)[ \.:;,]+ .*\]([ \.;])*$)", v.getText(), re.MULTILINE)
            if s :
                removed.write(s.group(2))
                removed.write("\n")
                text = s.group(1)
                if s.group(4) :
                    text+=s.group(4)
                v.string = text
    
    with open(outFilename, "w") as outFile:
        outFile.write(soup.prettify())