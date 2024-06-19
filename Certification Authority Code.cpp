#include<iostream>
#include<unordered_map>
#include <string>
#include <map>
#include <functional>
using namespace std;


class Certificate
{
    public:
    int Serial_number;
    int Signature_algorithm_id;
    string Issuer_name;
    int validityPeriod;
    string Subject_name;
    string SubjectPublicKey;
    string Signature;
    public:
    static int number;

    void generate(string name,string key)
    {
        Serial_number=number++;
        Signature_algorithm_id=31;
        Issuer_name="lavish";
        validityPeriod=20102003;
        Subject_name=name;
        SubjectPublicKey=key;
        string concate=to_string(Serial_number)+to_string(Signature_algorithm_id)+Issuer_name+to_string(validityPeriod)+Subject_name+(SubjectPublicKey);
        hash<string> hash_obj;
        Signature=hash_obj(concate);
    }
};

int Certificate::number=0;

class CertificationAuthority
{   private:
    int public_key;
    int private_key;
    static unordered_map<string,string> data;
    public:
    Certificate c;
     void issueCertificate(string name, string key) {
        
        data[name] = key;
        c.generate(name, key);
    }
};
unordered_map<string, string> CertificationAuthority::data;

class Bob
{
    private:
    int private_key;
    string public_key;
    public:
string bob;
    void requestCertificateFromCA(CertificationAuthority &ca) {
        cout << "Enter public key: ";
        cin >> public_key;
        cout<<"Issuing the certificate"<<endl;
         ca.issueCertificate(bob, public_key);
    }   
};

class Alice{
    public:
Certificate c;
  
  void getCertificate(Certificate l)
  {
    c=l;
  }

  bool verifySignature(string &Computed_signature) {
        // Recalculate the hash based on certificate data
        std::string concate = to_string(c.Serial_number) + to_string(c.Signature_algorithm_id) + c.Issuer_name
            + to_string(c.validityPeriod) + c.Subject_name + (c.SubjectPublicKey);

        hash<string> hash_obj;
        Computed_signature = hash_obj(concate);

        // Compare the computed signature with the stored signature
        return Computed_signature == c.Signature;
    }


};




int main(){
string yo;
string bobname;
cout<<endl;
cout<<"Do you want to create a certificate"<<endl;         
cin>>yo;
if(yo=="yes")
{
    Bob b;
  cout<<"Enter the Bob name"<<endl;
  cin>>bobname;
   b.bob = bobname;
  CertificationAuthority ca;
  b.requestCertificateFromCA(ca);

  Certificate c=ca.c;
  string g="";
  cout<<"Sending the certificate to Alice"<<endl;
  Alice a;
  a.getCertificate(c);
  if(a.verifySignature(g))
  {
    cout<<"verified";
  }
  else
  {
    cout<<"Not verified";
  }
}
else{
    cout<<"Meet you soon ";
}
return 0;
}