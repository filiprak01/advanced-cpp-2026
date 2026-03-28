#include <iostream>
#include <string>

#include <stdexcept>
class Osoba {
private:
int wiek;
double wzrost;
public:
Osoba(int _wiek, double _wzrost){
if (_wiek <= 0){
throw std::invalid_argument("Wiek musi byc wiekszy od 0 ");
}
if (_wzrost <= 0 ) {
throw std::invalid_argument("Wzrost musi byc wiekszy od 0");
}
wiek = _wiek;
wzrost = _wzrost;
}
};

using namespace std;

class CCzlonekPartii {
protected:
    std::string slogan;
public:
    virtual void setSlogan(std::string _s) { slogan = _s; }
    void mow(std::string _co) { cout << slogan << " " << _co; }
};

class CCzlonekPartiiPochodna : public CCzlonekPartii{
public:
void mow(std::string _co){
std::cout<<"Wina wiadomo kogo ";
CCzlonekPartii::mow(_co);
}
};

class CTowar {   
private:     
  int ilosc;     
  std::string nazwa;   
public:     
  CTowar() { nazwa = ""; ilosc = 0; }     
  void setNazwa(std::string _n) { nazwa = _n; }    
  void setIlosc(int _i) { ilosc = _i; }     
  void pisz() { cout << nazwa << ", ilosc: " << ilosc; }     
  CTowar operator+(int increase) const{
    CTowar ret = *this;
    ret.ilosc += increase;
    return ret;
  }  
}; 
