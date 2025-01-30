#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <algorithm>
#include <sys/stat.h>
#include <ctime>
#include <sstream>


/*
bool tmpignore(std::string& mainpath, std::string& dirpath) {
	std::string ignore1 = mainpath + "\\SCMultiDir";
	std::string ignore2 = mainpath + "\\x64";

	if ((dirpath == ignore1) || (dirpath == ignore2)) {
		return true;
	}
	else {
		return false;
	}
}
*/

bool ishidden(std::string& dirpath) {

	std::wstring wdirpath = std::wstring(dirpath.begin(), dirpath.end());
	LPCWSTR lpcdirpath = wdirpath.c_str();
	DWORD attributes = GetFileAttributes(lpcdirpath);

	if ((attributes & FILE_ATTRIBUTE_HIDDEN) || (attributes & FILE_ATTRIBUTE_SYSTEM)) {
		return true;
	}
	else {
		return false;
	}
}

std::string getfolder(std::string& folderpath) {
	std::string folder;

	if (folderpath.find("\\") != std::string::npos) {
		folder = folderpath.substr(folderpath.rfind("\\") + 1, folderpath.length());
	}
	else if (folderpath.find("/") != std::string::npos) {
		folder = folderpath.substr(folderpath.rfind("/") + 1, folderpath.length());
	}
	else {
		std::cout << "ERROR getting folder information; function getfolder" << std::endl;
	}

	return folder;
}

bool isHtml(std::string& check) {

	std::string checkthis = check;

	if (checkthis.length() >= 4) {
		std::string checking = checkthis.substr(checkthis.length() - 4);
		if (checking == "html") {
			return true;
		}
		else {
			return false;
		}
	}
	else {

		return false;
	}

}

std::string genericCull(std::string& input, std::string& start, std::string& end) {

	std::string cull = input.substr(input.find(start) + start.length());
	std::string cull1 = cull.substr(0, cull.find(end));
	return cull1;
}

std::string getTitle(std::string& fileContent) {

	if (fileContent.find("<title>") != std::string::npos) {
		std::string start = "<title>";
		std::string end = ": ";

		std::string title = genericCull(fileContent, start, end);

		return title;
	}

	else {
		std::cout << "ERROR in function getTitle" << std::endl;
		return "ERROR";
	}
}

std::string getContent(std::string& mainpath, std::string& filepath) {

	int test = 0;

	std::string fileContent;

	if ((filepath.length() >= 255) || (test == 1)) {
		std::string contenttmp = mainpath + "\\content.tmp";
		std::string genctmp = "@copy \"" + filepath + "\" \"" + contenttmp + "\" > NUL";
		int genctmpcmd = system(genctmp.c_str());

		std::fstream f(contenttmp, std::fstream::in);
		getline(f, fileContent, '\0');
		f.close();

		char temp[1024];
		strcpy_s(temp, contenttmp.c_str());
		std::filesystem::remove(temp);
	}
	else {
		std::fstream f(filepath, std::fstream::in);
		getline(f, fileContent, '\0');
		f.close();
	}
	return fileContent;
}

std::string getDirTitle(std::string& mainpath, std::string& filepath) {
	std::string fileContent = getContent(mainpath, filepath);
	std::string title = getTitle(fileContent);
	return title;
}

std::string checkSpecial(std::string text) {
	std::string t = text;
	int loop = 0;
	while (loop == 0) {
		if (t.find("&#39;") != std::string::npos) {
			std::string r = "&#39;";
			t.replace(t.find(r), r.size(), "'");
		}
		else if (t.find("&#34;") != std::string::npos) {
			std::string r = "&#34;";
			t.replace(t.find(r), r.size(), "'");
		}
		else if (t.find("&amp;") != std::string::npos) {
			std::string r = "&amp;";
			t.replace(t.find(r), r.size(), "&");
		}
		else if (t.find("\\") != std::string::npos) {
			replace(t.begin(), t.end(), '\\', '-');
		}
		else if (t.find("/") != std::string::npos) {
			replace(t.begin(), t.end(), '/', '-');
		}
		else if (t.find(":") != std::string::npos) {
			replace(t.begin(), t.end(), ':', ';');
		}
		else if (t.find("\"") != std::string::npos) {
			std::string r = "\"";
			t.replace(t.find(r), r.size(), "''"); \
		}
		else {
			loop = 1;
		}
	}
	int loop1 = 0;
	while (loop1 == 0) {
		if (t.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&()_-+={[}];',. ")
			!= std::string::npos) {

			std::size_t rfind =
				t.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~`!@#$%^&()_-+={[}];',. ");
			std::string r = t.substr(rfind, 1);
			t.replace(t.find(r), r.size(), "[]");

		}
		else {
			loop1 = 1;
		}
	}
	if (t == "") {
		t = "error getting story title";
	}

	return t;
}

bool dir_exists(std::string& dirpath) {
	if (std::filesystem::exists(dirpath)) {

		return true;
	}
	else {
		return false;
	}
}

void createFileF(std::string& diroutpath, std::string tmpinpath, std::string filenamewext) {
	int test = 0;

	std::string newfilepath = diroutpath + "\\" + filenamewext;
	if ((newfilepath.length() >= 255) || (test == 1)) {
		std::string fileContent;

		if (tmpinpath.length() >= 255) {
			//this should never happen
			std::cout << "error in createfile: tmp filepath too long" << std::endl;
		}
		else {
			std::fstream f(tmpinpath, std::fstream::in);
			getline(f, fileContent, '\0');
			f.close();
		}


		std::string longpath = "\\\\?\\" + newfilepath;
		std::wstring wlongpath = std::wstring(longpath.begin(), longpath.end());
		LPCWSTR llongpath = wlongpath.c_str();

		HANDLE createdfile = CreateFileW(llongpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);

		CloseHandle(createdfile);

		std::string cpy2file = "@copy \"" + fileContent + "\" \"" + newfilepath + "\" > NUL";
		int cpycmd = system(cpy2file.c_str());

	}
	else {
		std::string fileContent;
		std::fstream f(tmpinpath, std::fstream::in);
		getline(f, fileContent, '\0');
		f.close();

		std::ofstream newfile;
		newfile.open(newfilepath);
		newfile << fileContent << std::endl;
		newfile.close();
	}
}

void printOutlineHead(std::ofstream& thisFile) {
	thisFile << "<!--autogenerated-->" << std::endl;
	thisFile << "<!DOCTYPE html>" << std::endl;
	thisFile << "<html>" << std::endl;
	thisFile << "<head>" << std::endl;
	thisFile << "        <title>Story Outline</title>" << std::endl;
	thisFile << "        <style>" << std::endl;
	thisFile << "            body {" << std::endl;
	thisFile << "                background: #e37530;" << std::endl;
	thisFile << "            }" << std::endl;
	thisFile << "            .content {" << std::endl;
	thisFile << "                    margin: 0 auto;" << std::endl;
	thisFile << "                    padding: 5px 10px;" << std::endl;
	thisFile << "                    display: block;" << std::endl;
	thisFile << "                    background: #ededed;" << std::endl;
	thisFile << "                    font-family: TimesNewRoman;" << std::endl;
	thisFile << "                    border: 5px solid black;" << std::endl;
	thisFile << "					border-radius: 25px;" << std::endl;
	thisFile << "					box-shadow: 4px 6px 15px 2px #de8739;" << std::endl;
	thisFile << "            }" << std::endl;
	thisFile << "            .content ul {" << std::endl;
	thisFile << "                    padding: 0;" << std::endl;
	thisFile << "                    list-style: none;" << std::endl;
	thisFile << "                    font-family: TimesNewRoman;" << std::endl;
	thisFile << "            }" << std::endl;
	thisFile << "            .content h1 {" << std::endl;
	thisFile << "            		text-shadow: -1px 1px 1px #b6bdbf;" << std::endl;
	thisFile << "            }" << std::endl;
	thisFile << "        </style>" << std::endl;
	thisFile << "</head>" << std::endl;
	thisFile << "<div class = \"content\">" << std::endl;
	thisFile << "<h1>Story outline</h1>" << std::endl;
	thisFile << "<ul>" << std::endl;
}

void printChapHead(std::string& scratch) {

	std::ofstream tempfile;
	tempfile.open(scratch, std::ios_base::app);


	tempfile << "<!--autogenerated-->" << std::endl;
	tempfile << "<!DOCTYPE html>" << std::endl;
	tempfile << "<html>" << std::endl;
	tempfile << "<head>" << std::endl;
	tempfile << "   <style type=\"text/css\"><!--" << std::endl;
	tempfile << "body {" << std::endl;
	tempfile << "	background: #e37530;" << std::endl;
	tempfile << "	width:1000px;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "div.spacer {" << std::endl;
	tempfile << "	width: 90%;" << std::endl;
	tempfile << "	margin: 0 auto;" << std::endl;
	tempfile << "	height: 1px;" << std::endl;
	tempfile << "	border:1px solid black;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "div.content {" << std::endl;
	tempfile << "	display: flex;" << std::endl;
	tempfile << "	position:absolute;" << std::endl;
	tempfile << "	width: 921px;" << std::endl;
	tempfile << "	margin-left:191px;" << std::endl;
	tempfile << "	padding:5px 10px;" << std::endl;
	tempfile << "	display: block;" << std::endl;
	tempfile << "	background: #ededed;" << std::endl;
	tempfile << "	font-family: Arial;" << std::endl;
	tempfile << "	border: 5px solid black;" << std::endl;
	tempfile << "	border-radius: 25px;" << std::endl;
	tempfile << "	box-shadow: 4px 6px 15px 2px #de8739;" << std::endl;
	tempfile << "	word-wrap:break-word;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "div.content ul {" << std::endl;
	tempfile << "	list-style: none;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "h1 {" << std::endl;
	tempfile << "	text-align: center;" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 25px;" << std::endl;
	tempfile << "	text-shadow: -1px 1px 1px #b6bdbf;" << std::endl;
	tempfile << "	margin-bottom: -15px;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "p.story {" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 16px;" << std::endl;
	tempfile << "	text-shadow: 0px 0px 2px #b6bdbf;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "p.extraneous {" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 16px;" << std::endl;
	tempfile << "	text-shadow: 0px 0px 2px #b6bdbf;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "p.link {" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 14px;" << std::endl;
	tempfile << "	text-shadow: 0px 0px 2px #b6bdbf;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "a.fakelink {" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 14px;" << std::endl;
	tempfile << "	text-shadow: 0px 0px 2px #c9b2fd;" << std::endl;
	tempfile << "	cursor: pointer;" << std::endl;
	tempfile << "   color: #0070E0;" << std::endl;
	tempfile << "   text-decoration: underline;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << "" << std::endl;
	tempfile << "a:link {" << std::endl;
	tempfile << "	font-family: TimesNewRoman;" << std::endl;
	tempfile << "	font-size: 14px;" << std::endl;
	tempfile << "	text-shadow: 0px 0px 5px #c9b2fd;" << std::endl;
	tempfile << "}" << std::endl;
	tempfile << " --></style>" << std::endl;
	tempfile << "" << std::endl;

	tempfile.close();
}

std::string cullstart(std::string& input, std::string& start) {
	std::string cull = input.substr(input.find(start) + start.length());
	return cull;
}

std::string getChapterName(std::string& fileContent) {

	if (
		(fileContent.find(">Chapter") == std::string::npos) ||
		(fileContent.find("<big><big><b>") == std::string::npos) ||
		(fileContent.find("</b></big></big>") == std::string::npos)
		) {
		std::cout << "ERROR in function getchapter" << std::endl;
		return "ERROR";
	}
	else {

		std::string firstcull = "title=\"Created";
		std::string secondcull = "</b>";

		std::string cull1 = genericCull(fileContent, firstcull, secondcull);
		std::string thirdcull = "<b>";
		std::string cull2 = cullstart(cull1, thirdcull);

		return cull2;
	}
}

std::string getOwner(std::string& fileContent) {

	if ((fileContent.find("All rights reserved.</b><br><i>") == std::string::npos) &&
		(fileContent.find("<a title=\"Username: ") == std::string::npos)
		) {
		std::cout << "ERROR in function getOwner" << std::endl;
		return "ERROR";
	}

	else if (fileContent.find("All rights reserved.</b><br><i>") == std::string::npos) {

		std::string start = "<a title=\"Username: ";
		std::string end = "</a>";
		std::string cull = genericCull(fileContent, start, end);

		std::string start2 = ">";
		std::string cull2 = cullstart(cull, start2);

		return cull2;
	}
	else {

		std::string A = "All rights reserved.</b><br><i>";

		int i = fileContent.find(A);
		std::string cull = fileContent.substr(i + A.length());
		cull.erase(cull.find(' '));

		return cull;
	}
}

std::string cullend(std::string& input, std::string& end) {
	std::string cull = input.substr(0, input.find(end));
	return cull;
}

bool chapfilenumGood(std::string& chapfilenum) {
	if (chapfilenum.find_first_not_of("0123456789") != std::string::npos) {
		return false;
	}
	else{
		if (chapfilenum.length() <= 1){
			return false;
		}
		else {
			return true;
		}
	}
}

std::string getChoice(std::string& fileContent) {

	if (fileContent.find("This choice: <b>") != std::string::npos) {
		std::string start = ">This choice: <b>";
		std::string end = "</b>";
		std::string choice = genericCull(fileContent, start, end);
		return choice;
	}
	else {
		std::cout << "ERROR in function getChoice" << std::endl;
		return "ERROR";
	}
}

std::string getBackLink(std::string& fileContent) {

	if (
		(fileContent.find("\">This choice") == std::string::npos) ||
		(fileContent.find("\">Go Back") == std::string::npos)
		) {
		std::cout << "ERROR in function getBackLink" << std::endl;
		return "ERROR";
	}

	if (fileContent.find("/map/") != std::string::npos) {

		std::string start = "\">This choice";
		std::string cull = cullstart(fileContent, start);

		std::string next = "/map/";
		std::string last = "\">Go Back";
		std::string linknumber = genericCull(cull, next, last);

		std::string fullLink = linknumber + ".html";

		return fullLink;
	}
	else if (fileContent.find("/map/") == std::string::npos) {

		std::string start = "\">This choice";
		std::string end = "\">Go Back";
		std::string cull = genericCull(fileContent, start, end);

		std::string remove = "\"";
		std::string cull1 = cull.substr(cull.find_last_of(remove) + 1);

		return cull1;
	}
	else {
		std::cout << "ERROR in function getBackLink" << std::endl;
		return "ERROR";
	}
}

std::string getChapNum(std::string& fileContent) {

	if (fileContent.find(">Chapter ") == std::string::npos) {
		std::cout << "ERROR in function getChapNum" << std::endl;
		return "ERROR";
	}

	else {
		std::string firstcull = "title=\"Created";
		std::string cull = fileContent.substr(fileContent.find(firstcull) + 15);

		std::string secondcull = ":";
		std::string cull1 = cullend(cull, secondcull);

		std::string thirdcull = "Chapter ";
		std::string cull2 = cullstart(cull1, thirdcull);

		return cull2;
	}
}

bool isGuestAuthor(std::string& fileContent) {
	std::string findThis = ">More by this author";

	if (fileContent.find(findThis) != std::string::npos) {
		return true;
	}
	else {
		return false;
	}
}

std::string getChapAuthor(std::string& fileContent) {

	std::string firstcull = "title=\"Created";
	std::string secondcull = "Where will this story go next";
	std::string cull = genericCull(fileContent, firstcull, secondcull);

	std::string thirdcull = "<a title=\"Username";
	std::string cull1 = cullstart(cull, thirdcull);

	std::string fourthcull = "pointer;\">";
	std::string fifthcull = "</a>";
	std::string cull2 = genericCull(cull1, fourthcull, fifthcull);

	return cull2;
}

std::string removebr(std::string story) {
	int loop = 0;
	std::string s = story;

	while (loop == 0) {
		if (s.find("<br>") != std::string::npos) {
			std::string r = "<br>";
			//s.replace(s.find(r), r.size(), "(#br#)");
			s.replace(s.find(r), r.size(), "");
		}
		else if (s.find("<br/>") != std::string::npos) {
			std::string r = "<br/>";
			//s.replace(s.find(r), r.size(), "(#br#)");
			s.replace(s.find(r), r.size(), "");
		}
		else if (s.find("</") != std::string::npos) {
			std::string r = "</";
			s.replace(s.find(r), r.size(), "(#«/#)");
		}
		else {
			loop = 1;
		}
	}
	return s;
}

std::string cullWEnds(std::string& input, std::string& start, std::string& end) {

	std::string startin = start;
	startin.resize(1);

	std::string cull = startin + input.substr(input.find(start));
	std::string cull1 = cull.substr(1, cull.find(end));

	return cull1;
}

std::string cullbetween(std::string& input, std::string& start, std::string& end) {
	std::string s = input;

	std::string toRemove = cullWEnds(s, start, end);
	size_t spos = s.find(toRemove);
	s.replace(spos, toRemove.length(), "");

	return s;

}

std::string restorebr(std::string story) {
	int loop = 0;
	std::string s = story;

	while (loop == 0) {
		/*
		if (s.find("(#br#)") != std::string::npos) {
			std::string r = "(#br#)";
			s.replace(s.find(r), r.size(), "<br>");
		}
		else if (s.find("(#«/#)") != std::string::npos) {
		*/
		if (s.find("(#«/#)") != std::string::npos) {
			std::string r = "(#«/#)";
			s.replace(s.find(r), r.size(), "</");
		}
		else {
			loop = 1;
		}
	}
	return s;
}

std::string fixFormat(std::string& story){
	
	std::string s = story;
	std::string nl = "\n";
	std::ostringstream temp;
	
	int loop = 1;
	int update = 0;
	std::string ti;
	while (loop == 1) {
		int found = 0;
		if (s.find(nl) != std::string::npos) {
			found = 1;
			std::string strB = s.substr(s.find(nl) + nl.length());
			std::string strF;
			if (strB.length() >= 6) {
				strF = s.substr(s.find(nl) + nl.length(),  5);
			}
			else {
				strF = strB;
			}

			//std::string strF = s.substr(s.find(nl) + nl.length() + 5);
			if (strF.find("<br>") == std::string::npos) {
				std::string r = "<br>";
				s.replace(s.find(nl), nl.size(), r);
			}
		std::string br = "<br>";
		std::string sn = s.substr(0, s.find(br));
		temp << sn << br << std::endl;
		std::string so = s.substr(s.find(br) + br.length());
		s = so;
		}
		std::string t = temp.str();
		if ((s.find(nl) == std::string::npos) && (found == 1)) {
			ti = t + s;
			update = 1;
		}
		if ((s.find(nl) == std::string::npos) && (found == 0)) {
			loop = 0;
		}
	}
	
	if (update == 1){
		s = ti;
	}
	
	return s; 
	
}

std::string cleanstory(std::string& story) {
	std::string s = removebr(story);
	int loop = 0;

	while (loop == 0) {
		if (s.find("<") != std::string::npos) {
			std::string findstart = "<";
			std::string findend = ">";
			std::string craptype = genericCull(s, findstart, findend);
			std::string crap1 = "<" + craptype + ">";
			std::string crap2 = "(#«/#)" + craptype + ">";
			if (s.find(crap1) != std::string::npos) {
				s.replace(s.find(crap1), crap1.size(), "");
			}
			if (s.find(crap2) != std::string::npos) {
				s.replace(s.find(crap2), crap2.size(), "");
			}
		}
		else {
			loop = 1;
		}
	}
	std::string s1 = restorebr(s);
	
	std::string s2 = fixFormat(s1);
	
	s = s2;

	return s;
}

std::string konaStory(std::string& fileContent) {

	std::string start = "KonaBody\">";
	std::string end = "</div>";
	std::string cull = genericCull(fileContent, start, end);

	std::string story = cleanstory(cull);
	return story;
}

std::string morebyStory(std::string& fileContent) {

	std::string r1 = "More by this author";
	std::string r2 = "Where will this story go next";
	std::string cull = genericCull(fileContent, r1, r2);

	std::string r3 = "<div class=";
	std::string r4 = "</div>";
	std::string cull1 = genericCull(cull, r3, r4);

	std::string r5 = ">";
	std::string cull2 = cullstart(cull1, r5);

	std::string story = cleanstory(cull2);
	return story;
}

std::string chapterStory(std::string& fileContent) {

	std::string firstcullstart = ">Chapter ";
	std::string cull = cullstart(fileContent, firstcullstart);

	if (cull.find("Where will this story go next") == std::string::npos) {
		std::cout << "ERROR in getStory: chapterStory" << std::endl;
		return "ERROR";
	}

	std::string firstcullend = "Where will this story go next";
	std::string cull1 = cullend(cull, firstcullend);

	if (cull1.find("<div class=") == std::string::npos) {
		std::cout << "ERROR in getStory: chapterStory" << std::endl;
		return "ERROR";
	}

	std::string secondcullstart = "<div class=";
	std::string cull2 = cullstart(cull1, secondcullstart);

	if (cull2.find("</div>") == std::string::npos) {
		std::cout << "ERROR in getStory: chapterStory" << std::endl;
		return "ERROR";
	}

	std::string secondcullend = "</div>";
	std::string cull3 = cullend(cull2, secondcullend);

	std::string lastcull = ">";
	std::string cull4 = cullstart(cull3, lastcull);

	std::string story = cleanstory(cull4);

	return story;
}

std::string getStory(std::string& fileContent) {

	if (fileContent.find("KonaBody\">") != std::string::npos) {

		//cout << "kona" << endl;

		std::string ineedhelp = konaStory(fileContent);
		return ineedhelp;
	}

	else if (fileContent.find("More by this author") != std::string::npos) {

		//cout << "moreby" << endl;

		std::string ineedhelp = morebyStory(fileContent);
		return ineedhelp;
	}

	else if (fileContent.find(">Chapter ") != std::string::npos) {

		//cout << "chapter" << endl;

		std::string ineedhelp = chapterStory(fileContent);
		return ineedhelp;
	}


	else {
		std::cout << "ERROR in function getStory" << std::endl;
		return "ERROR";
	}
}

void printEnd(std::string& scratch) {
	
	std::ofstream scratchS;
	scratchS.open(scratch, std::ios_base::app);

	scratchS << "			<p class=\"extraneous\" align=\"center\"><i><b><big>THE END.</i></b></big></p>" << std::endl;
	scratchS << "			<p class=\"extraneous\" align=\"center\">You have come to the end of the story.</p>" << std::endl;
	scratchS.close();
}

bool checkempty(std::string& cull1) {

	std::string checkifempty1 = cull1.substr(0, cull1.find("href="));

	if (checkifempty1.find("*") == std::string::npos) {
		return false;
	}
	else if (checkifempty1.find("*") != std::string::npos) {
		return true;
	}
	else {
		std::cout << "ERROR in function checkemtpy" << std::endl;
	}

}

std::string checkifhtml(std::string& numcheck) {
	std::string fixed;

	if (numcheck.length() >= 4) {
		if (isHtml(numcheck) == true) {
			std::string cull = ".";
			fixed = cullend(numcheck, cull);
			return fixed;
		}
		else {
			return numcheck;
		}
	}
	else return numcheck;
}

void printChapters(std::string& scratch, std::string& num, std::string& name, std::string& thischapnum, int& emp) {

	std::ofstream scratchS;
	scratchS.open(scratch, std::ios_base::app);

	std::string num1 = checkifhtml(num);
	std::string chapnum = num1.substr(num1.length() - 1);

	if (emp != 1) {
		std::string linknum = thischapnum + chapnum;
		scratchS << "			<div><p class=\"link\">" << chapnum << ". <a href=\"" << linknum << ".html\">" << name << "</a></p></div>" << std::endl;
	}
	else if (emp == 1) {
		scratchS << "			<div><p class=\"link\">" << chapnum << ". <a class=\"fakelink\">" << name << "</a> *</</p></div>" << std::endl;
	}
	else {
		std::cout << "ERROR in function printChapters" << std::endl;
	}

	scratchS.close();
}

void sendChapPrint(std::string& cull1, std::string& scratch, std::string& thischapnum) {
	std::string choicinf;
	std::string chapnum;
	std::string chapname;
	int emp;

	std::string find1 = "</a>";
	choicinf = cullend(cull1, find1);

	std::string find2 = "\">";
	chapnum = cullend(choicinf, find2);
	chapname = cullstart(choicinf, find2);

	if (checkempty(cull1) == true) {
		emp = 1;
		printChapters(scratch, chapnum, chapname, thischapnum, emp);
	}
	else if (checkempty(cull1) == false) {
		emp = 0;
		printChapters(scratch, chapnum, chapname, thischapnum, emp);
	}
	else {
		std::cout << "ERROR in function sendChapPrint" << std::endl;
	}
}

void getchapterLoop(std::string& scratch, std::string& input, std::string& delim, std::string& thischapnum) {

	std::string delimA = delim;
	std::string cull = input;

	std::string cull1 = cullstart(cull, delimA);

	std::string cull2;

	int loop = 1;
	while (loop == 1) {
		if (cull1.find(delimA) == std::string::npos) {
			sendChapPrint(cull1, scratch, thischapnum);
			loop = 0;
		}
		else {
			sendChapPrint(cull1, scratch, thischapnum);
			cull2 = cullstart(cull1, delimA);
			cull1 = cull2;
		}
	}
	return;
}

void getChapters(std::string& scratch, std::string& fileContent, std::string& thischapnum) {
	std::string delim;

	if (fileContent.find("You have come to the end of the story") != std::string::npos) {
		printEnd(scratch);
	}

	else if (fileContent.find("end_of_choices") == std::string::npos) {
		std::cout << "ERROR in function getChapters" << std::endl;
	}

	else if (fileContent.find("You have the following choice") != std::string::npos) {
		std::string start = "You have the following choice";
		std::string end = "end_of_choices";
		std::string cull = genericCull(fileContent, start, end);

		if (cull.find("map/") != std::string::npos) {
			delim = "map/";
			getchapterLoop(scratch, cull, delim, thischapnum);
		}

		else if (cull.find("href") != std::string::npos) {
			delim = "href=\"";
			getchapterLoop(scratch, cull, delim, thischapnum);
		}
	}

	else {
		std::cout << "ERROR in function getChapters" << std::endl;
	}
}

void populatefile(std::string& fileContent, std::string& scratch, std::string& filename, std::string& outlinetmppath) {

	printChapHead(scratch);

	std::ofstream scratchS;
	scratchS.open(scratch, std::ios_base::app);

	std::string chapterName = getChapterName(fileContent);
	scratchS << "	<title>" << chapterName << "</title>" << std::endl;
	scratchS << "</head>" << std::endl;
	scratchS << "<body>" << std::endl;
	scratchS << "" << std::endl;
	scratchS << "		<div class = \"content\">" << std::endl;

	std::string title = getTitle(fileContent);
	scratchS << "		<h1>" << title << "</h1>" << std::endl;

	std::string owner = getOwner(fileContent);
	scratchS << "		<p class=\"extraneous\" align=\"center\">by: " << owner << "</p>" << std::endl;
	scratchS << "" << std::endl;
	scratchS << "		<p class=\"extraneous\">" << std::endl;

	std::string ext = ".html";
	std::string chapfilenum = cullend(filename, ext);
	if (chapfilenumGood(chapfilenum)){
		std::string choice = getChoice(fileContent);
		std::string link = getBackLink(fileContent);

		scratchS << "		This Choice: <b>" << choice << "</b> - <a href=\"" << link << "\">Go Back</a>" << std::endl;
		scratchS << "		<br><br>" << std::endl;
	}

	std::string num = getChapNum(fileContent);
	scratchS << "		Chapter " << num << ": <big><big><b>" << chapterName << "</b></big></big>" << std::endl;
	scratchS << "		<br>" << std::endl;

	if (isGuestAuthor(fileContent)) {
		std::string author = getChapAuthor(fileContent);
		scratchS << "		&nbsp By: <big>" << author << "</big>" << std::endl;
	}
	scratchS << "		</p>" << std::endl;
	scratchS << "" << std::endl;

	std::string story = getStory(fileContent);
	scratchS << "		<p class=\"story\">" << story << std::endl;
	scratchS << "		</p>" << std::endl;
	scratchS << "		<p class=\"extraneous\">" << std::endl;
	scratchS << "		What Happens next?" << std::endl;
	scratchS << "		</p>" << std::endl;
	scratchS << "		<ol>" << std::endl;
	scratchS << "			<p class=\"extraneous\">You have these choices:</p>" << std::endl;

	scratchS.close();

	getChapters(scratch, fileContent, chapfilenum);

	scratchS.open(scratch, std::ios_base::app);

	scratchS << "		</ol>" << std::endl;
	scratchS << "		<br><br><div class=\"spacer\"></div><br>" << std::endl;
	scratchS << "		<p align=\"center\"><a href=\"00_outline.html\">Story Outline</a></p>" << std::endl;
	scratchS << "	</div>" << std::endl;
	scratchS << "" << std::endl;
	scratchS << "</body>" << std::endl;
	scratchS << "</html>" << std::endl;
	scratchS << "<!--end-->" << std::endl;

	scratchS.close();

	std::string outlineentry;
	std::string visiblelink = chapfilenum + " - " + chapterName;

	std::ofstream outlinetmp;
	outlinetmp.open(outlinetmppath, std::ios_base::app);

	if (visiblelink.length() > 158) {
		std::string newnum = chapfilenum.substr(0, 155 - chapterName.length()) + "...";
		outlineentry = "<li><a href=\"" + filename + "\">" + newnum + " - " + chapterName + "</a></li>";
		outlinetmp << outlineentry << std::endl;
	}
	else {
		outlineentry = "<li><a href=\"" + filename + "\">" + visiblelink + "</a></li>";
		outlinetmp << outlineentry << std::endl;
	}

	outlinetmp.close();

	std::cout << "cleaned: " << filename << std::endl;
}

void replacefile(std::string& scratch, std::string& filepath) {
	
	if (filepath.length() >= 255) {

		std::string clearfile = "copy / y nul \"" + filepath + "\" >NUL";
		int clearcmd = system(clearfile.c_str());

		std::string updatefile = "@copy \"" + scratch + "\" \"" + filepath + "\" > NUL";
		int updatecmd = system(updatefile.c_str());

	}
	else {
		std::fstream f(scratch, std::fstream::in);
		std::string newContent;
		getline(f, newContent, '\0');
		f.close();

		std::ofstream replace;
		replace.open(filepath, std::ios_base::trunc);
		replace << newContent << std::endl;
		replace.close();
	}
}

void doWork(std::string& tempdir, std::string& dirpath, std::string& filepath, std::string& filename, std::string& olddir, std::string& outlinetmppath, std::string& tempdirold) {
	int test = 0;
	std::string oldtmp = tempdir + "\\old.tmp";
	std::string scratch = tempdir + "\\scratch.tmp";
	std::string fileContent = getContent(dirpath, filepath);

	std::ofstream oldtmpS;
	oldtmpS.open(oldtmp);
	oldtmpS << fileContent << std::endl;
	oldtmpS.close();

	createFileF(olddir, oldtmp, filename);
	createFileF(tempdirold, oldtmp, filename);

	char oldtmpc[1024];
	strcpy_s(oldtmpc, oldtmp.c_str());
	std::filesystem::remove(oldtmpc);

	std::ofstream scratchS;
	scratchS.open(scratch);
	scratchS.close();
	
	populatefile(fileContent, scratch, filename, outlinetmppath);

	replacefile(scratch, filepath);
	char scratchc[1024];
	strcpy_s(scratchc, scratch.c_str());
	std::filesystem::remove(scratchc);
}

void printOutlineFoot(std::ofstream& thisFile) {
	thisFile << "</ul>" << std::endl;
	thisFile << "</div>" << std::endl;
	thisFile << "</body>" << std::endl;
	thisFile << "<!--end-->" << std::endl;
}

std::string getDate() {

	const int MAXLEN = 80;
	char dateStr[MAXLEN];

	// Get the current time
	time_t rawTime;
	time(&rawTime);

	// Convert the time to a struct tm
	struct tm now;
	localtime_s(&now, &rawTime);

	// Format the date string as mm/dd/yy
	strftime(dateStr, MAXLEN, "%m/%d/%y", &now);

	std::ostringstream gendate;
	gendate << dateStr;

	return gendate.str();
}

void SC(std::string& mainpath, std::string& dirpath, int& gottmppath, std::string& tempdir) {
	int except = 0;
	int oldexpt = 0;

	std::string gendate;

	const std::filesystem::path p1 = dirpath;

	std::string ogfoldername = getfolder(dirpath);
	std::string olddir = dirpath + "\\old_" + ogfoldername;

	std::string tempdirold = tempdir + "\\old_" + ogfoldername;
	std::string tzipdirold = tempdir + "\\old_" + ogfoldername + ".zip";

	std::string movezipto = dirpath + "\\old_" + ogfoldername + ".zip";
	std::string zipname = "old_" + ogfoldername + ".zip";

	std::string zipOld = "powershell Compress-Archive -Path '" + tempdirold + "\\*' -DestinationPath '" + tzipdirold + "'";
	int zipoldcmd;

	std::string movezip = "move \"" + tzipdirold + "\" \"" + movezipto + "\" > NUL";
	int movezipcmd;

	std::string removeolddir = "rmdir /s /q \"" + olddir + "\"";
	int remolddircmd;

	std::string removetmpolddir = "rmdir /s /q \"" + tempdirold + "\"";
	int remtoldcmd;


	std::wstring woldzip;
	LPCWSTR loldzip;

	std::wstring wmovezip;
	LPCWSTR lmovezip;

	std::string newdirname;
	std::string checkednewdir;
	std::string newdirpath;

	std::wstring wdirpath;
	LPCWSTR ldirpath;

	std::wstring wnewdirpath;
	LPCWSTR lnewdirpath;

	int findauto = 0;
	int findhtml = 0;

	std::string ndp;
	int dirloop = 0;
	int dirnum = 1;

	std::ofstream outlinetmp;
	std::string outlinetmppath = tempdir + "\\outline.tmp";
	char outlinetmpc[1024];
	strcpy_s(outlinetmpc, outlinetmppath.c_str());

	std::ofstream finishedproof;
	std::string proofpath = dirpath + "\\autogen.inf";

	for (const auto& entry : std::filesystem::directory_iterator(p1)) {
		std::string filename = entry.path().filename().string();
		std::string filepath = entry.path().string();
		if (entry.is_regular_file()) {
			if (filename == "autogen.inf") {
				findauto = 1;
				break;
			}
		}
	}

	if (findauto != 0) {
		except = 1;
		goto end;
	}

	for (const auto& entry : std::filesystem::directory_iterator(p1)) {
		std::string filename = entry.path().filename().string();
		std::string filepath = entry.path().string();
		if (entry.is_regular_file()) {
			if (filename == "autogen.inf") {
				findauto = 1;
				break;
			}
			else if (isHtml(filename) == true) {
				findhtml = 1;
				newdirname = getDirTitle(mainpath, filepath);
				break;
			};
		}
	}

	if (findhtml == 0)  {
		except = 1;
		goto end;
	}

	checkednewdir = checkSpecial(newdirname);

	newdirpath = mainpath + "\\" + checkednewdir;
	ndp = newdirpath;
	while (dirloop == 0) {
		if (dir_exists(ndp)) {
			std::string getnum = std::to_string(dirnum);
			ndp = newdirpath + " (" + getnum + ")";
			dirnum++;
		}
		else {
			dirnum = 1;
			dirloop = 1;
		}
	}

	newdirpath = ndp;

	if (dir_exists(olddir)) {
		except = 1;
		oldexpt = 1;
		goto end;
	}

	std::filesystem::create_directory(olddir);
	std::filesystem::create_directory(tempdirold);
	outlinetmp.open(outlinetmppath);
	printOutlineHead(outlinetmp);
	outlinetmp.close();
	
	for (const auto& entry : std::filesystem::directory_iterator(p1)) {
		std::string filepath = entry.path().string();
		std::string filename = entry.path().filename().string();
		if (entry.is_regular_file()) {
			if (isHtml(filename) == true) {
				doWork(tempdir, dirpath, filepath, filename, olddir, outlinetmppath, tempdirold);
			};
		}
	}
	outlinetmp.open(outlinetmppath, std::ios_base::app);

	printOutlineFoot(outlinetmp);
	outlinetmp.close();

	createFileF(dirpath, outlinetmppath, "00_outline.html");


	std::filesystem::remove(outlinetmpc);
	std::cout << "generated outline" << std::endl;


	zipoldcmd = system(zipOld.c_str());

	movezipcmd = system(movezip.c_str());

	
	if (dir_exists(movezipto) == true) {
		remolddircmd = system(removeolddir.c_str());
		std::cout << "created backup: " + zipname << std::endl;
	}
	else {
		std::cout << "error: couldn't compress backup, leaving uncompressed" << std::endl;
	}
	remtoldcmd = system(removetmpolddir.c_str());

	gendate = getDate();
	finishedproof.open(proofpath);
	finishedproof << "Autogenerated by StoryCleanerMultiDirectory on " << gendate << ", from " << ogfoldername << std::endl;
	finishedproof << "Original, unprocessed files are stored in old_" << ogfoldername << ".zip" << std::endl;
	finishedproof.close();
	

	wdirpath = std::wstring(dirpath.begin(), dirpath.end());
	ldirpath = wdirpath.c_str();

	wnewdirpath = std::wstring(newdirpath.begin(), newdirpath.end());
	lnewdirpath = wnewdirpath.c_str();

	MoveFile(ldirpath, lnewdirpath);

end:
	if (except != 0) {
		if (oldexpt != 0) {
			std::cout << "error: directory " << olddir << " already exists" << std::endl;
			std::cout << "aborting cleaning process for " << dirpath << std::endl;
		}
		else if (findauto !=0) {
			std::cout << "found autogen file, implying directory already cleaned. skipping" << std::endl;;
		}
		else if (findhtml == 0) {
			std::cout << "no html files found in directory. Skipping" << std::endl;
		}
		else {
			//shouldn't happen
			std::cout << "unknown error, aborting" << std::endl;
		}
	}
	return;
}



int main() {

	std::string temppath;
	int gottmppath = 0;
	const char* envVarName = "APPDATA";
	char* envVarValue = NULL;
	size_t sz = 0;

	if (_dupenv_s(&envVarValue, &sz, envVarName) == 0 && envVarValue != NULL) {
		gottmppath = 1;
		temppath =envVarValue;
	}

	std::string tempdir = temppath + "\\SCMultiDir";
	std::filesystem::create_directory(tempdir);

	std::filesystem::path p = std::filesystem::current_path();
	std::string path = p.generic_string();

	std::string mainpath;

	std::ofstream getpath;
	getpath.open("getpath.tmp");
	char getpathc[] = "getpath.tmp";
	getpath.close();

	for (const auto& entry : std::filesystem::directory_iterator(p)) {
		std::string filepath = entry.path().parent_path().string();
		std::string filename = entry.path().filename().string();
		if (filename == "getpath.tmp") {
			mainpath = filepath;
		}
	}
	remove(getpathc);


	for (const auto& entry : std::filesystem::directory_iterator(p)) {
		if (entry.is_directory()) {
			std::string dirpath = entry.path().string();
			
			//if ((ishidden(dirpath) == false) && (tmpignore(mainpath, dirpath) == false)) {
			if (ishidden(dirpath) == false){
				std::string dname = entry.path().filename().string();
				std::cout << "running cleaner in folder: " << dname << std::endl;
				std::cout << "" << std::endl;
				SC(mainpath, dirpath, gottmppath, tempdir);
				std::cout << "" << std::endl;
			}
		}
	}

	std::string removetmpdir = "rmdir /s /q \"" + tempdir + "\"";
	int remtdcmd = system(removetmpdir.c_str());

	std::cout << "" << std::endl;
	std::cout << "done" << std::endl;
	system("pause");
	return 0;
}