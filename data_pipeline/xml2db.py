import os
from bs4 import BeautifulSoup
import sqlite3

def xml2db (inFileName,outFileName):
	if os.path.exists(outFileName):
		os.remove(outFileName)

	connection = sqlite3.connect(outFileName)
	cur = connection.cursor()
	cur.execute("""CREATE TABLE "books" (
		"id"	INTEGER UNIQUE,
		"name"	TEXT,
		"chapterCount"	INTEGER,
		PRIMARY KEY('id')
	)""")
	cur.execute("""CREATE TABLE "verses" (
		"id"	INTEGER UNIQUE,
		"book_id"	INTEGER,
		"chapter"	INTEGER,
		"verse"	INTEGER,
		"text"	TEXT,
		PRIMARY KEY("id" AUTOINCREMENT)
	)""")


	with open(inFileName,"r") as data :
		soup = BeautifulSoup(data,"xml")

		books = soup.find_all("div",type="book")
		#The name of the book in xmlfile are not malagasy name and the format is not very reader friendly
		booknames = {1: 'Genesisy', 2: 'Eksodosy', 3: 'Levitikosy', 4: 'Nomery', 5: 'Deoteronomia', 6: 'Josoa', 7: 'Mpitsara', 8: 'Rota', 9: 'I Samoela', 10: 'II Samoela', 11: 'I Mpanjaka', 12: 'II Mpanjaka', 13: 'I Tantara', 14: 'II Tantara', 15: 'Ezra', 16: 'Nehemia', 17: 'Estera', 18: 'Joba', 19: 'Salamo', 20: 'Ohabolana', 21: 'Mpitoriteny', 22: "Tonon-kiran'i Solomona", 23: 'Isaia', 24: 'Jeremia', 25: 'Fitomaniana', 26: 'Ezekiela', 27: 'Daniela', 28: 'Hosea', 29: 'Joela', 30: 'Amosa', 31: 'Obadia', 32: 'Jona', 33: 'Mika', 34: 'Nahoma', 35: 'Habakoka', 36: 'Zefania', 37: 'Hagay', 38: 'Zakaria', 39: 'Malakia', 40: 'Matio', 41: 'Marka', 42: 'Lioka', 43: 'Jaona', 44: "Asan'ny Apostoly", 45: 'Romana', 46: 'I Korintiana', 47: 'II Korintiana', 48: 'Galatiana', 49: 'Efesiana', 50: 'Filipiana', 51: 'Kolosiana', 52: 'I Tesaloniana', 53: 'II Tesaloniana', 54: 'I Timoty', 55: 'II Timoty', 56: 'Titosy', 57: 'Filemona', 58: 'Hebreo', 59: 'Jakoba', 60: 'I Petera', 61: 'II Petera', 62: 'I Jaona', 63: 'II Jaona', 64: 'III Jaona', 65: 'Joda', 66: 'Apokalypsy'}

		bookId=0
		for book in books :
			bookId+=1
			chapters = book.find_all("div",type="chapter")
			chapterCount = len(chapters)
			print(booknames[bookId])
			cur.execute("INSERT INTO books(id,name,chaptercount) values(?,?,?)",(bookId,booknames[bookId],chapterCount))
			chapterId=0
			for chapter in chapters :
				chapterId+=1
				verses = chapter.find_all("seg")
				
				verseId=0
				for verse in verses :
					verseId+=1
					text = verse.text
					cur.execute("INSERT INTO verses(book_id,chapter,verse,text) values(?,?,?,?)",(bookId,chapterId,verseId,text.strip()))


	data.close()
	connection.commit()
	cur.close()
	connection.close()
