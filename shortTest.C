#include <iostream>

class FileAndStdOut {
public:
  FileAndStdOut() {};
  FileAndStdOut(const char *fileName) {
    Init(fileName);
  }
  void Init(const char *fileName) {
    fFile.open(fileName,std::ios::out);
  }
  void SetFileName(const char *fileName) { Init(fileName); }
  virtual ~FileAndStdOut() {};
  FileAndStdOut& operator<<(ostream& (*pfun)(ostream&))
  {
    if(is_open())
      pfun(fFile);
    pfun(std::cout);
    return *this;
  }

  bool is_open() { return fFile.is_open(); }
private:
  std::fstream fFile;
};

template<class T>
inline FileAndStdOut& operator<<(FileAndStdOut &s, T v)
{
  if(s.is_open())
    s.fFile << v;
  std::cout << v;
  return s;
}

void shortTest()
{
  FileAndStdOut myOut;
  //myOut << "Hello world!!" << std::endl;
  myOut << "Hello world!!\n";
  myOut << std::endl;
}
