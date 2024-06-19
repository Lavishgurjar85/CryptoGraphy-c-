#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
using namespace std;


string encryptXOR(const string &plaintext, const string &key)
{
string ciphertext = plaintext;
for (size_t i = 0; i < plaintext.size(); ++i)
{
ciphertext[i] ^= key[i % key.size()];
}
return ciphertext;
}

string decryptXOR(const string &ciphertext, const string &key)
{
return encryptXOR(ciphertext, key);
}

map<string, string> readUserPasswordsFromFile(const string &filename)
{
    map<string, string> userPasswords;
    ifstream file(filename);

    if (file.is_open())
    {
        std::string username, password;
        while (file >> username >> password)
        {
            userPasswords[username] = password;
        }
        file.close();
    }

    return userPasswords;
}

void storeUserPassword(const string &filename, const string &username, const string &password)
{
    std::ofstream file(filename, ios::app);
    if (file.is_open())
    {
        file << username << " " << password << endl;
        file.close();
        std::cout << "User created and stored successfully." << endl;
    }
    else
    {
        std::cerr << "Error opening file for writing." << endl;
    }
}

string generateNonce()
{
    string nonce(16, '\0');
    for (int i = 0; i < 16; ++i)
    {
        nonce[i] = static_cast<char>(rand() % 256);
    }
    return nonce;
}

class KDC
{
public:
    string Ra;
    string Bob;
    string alice;
    string Session_key;
    string Msg[5];
    
    void init(string r,string bob,string alice)
    {
        this->Ra=r;
        this->Bob=bob;
        this->alice=alice;
        Session_key=generateNonce();
    }
};

bool check( map<string, string> userPasswords,string alice,string bob,string &ka,string &kb)
{
    int l=0;
   auto it = userPasswords.find(bob);
    if (it != userPasswords.end()) {
      kb=it->second;
        l=1;
    }
    auto it2=userPasswords.find(alice);
    if (it2 != userPasswords.end()) {
      ka=it2->second;
        l=2;
    }
    if(l==2)
    return true;
    else
    return false;

}

class Alice
{
public:
    string name;
    string bob;
    string random1;
    string session_key;
    string shared_key1;
    KDC k;
    Alice()
    { }
    Alice(string bob, string random1, string alice)
    {
        this->name = alice;
        this->bob = bob;
        this->random1 = random1;
    }

    bool sendRequestToKDC(string filename,string &ka,string &kb)
    {
        k.init(random1,bob,name);
        string msg="";
        string r=k.Ra;
        string bob=k.Bob;
        string alice=k.alice;
        string session=k.Session_key;
        map<string, std::string> userPasswords = readUserPasswordsFromFile(filename);
        if(check(userPasswords,alice,bob,ka,kb))
        {
            k.Msg[0]=encryptXOR(r,ka);
            k.Msg[1]=encryptXOR(bob,ka);
            k.Msg[2]=encryptXOR(session,ka);
            k.Msg[3]=encryptXOR(encryptXOR(alice,kb),ka);
            k.Msg[4]=encryptXOR(encryptXOR(session,kb),ka);
          return true;
        }
        else
        return false;
    }


string sendResponseToBob(string msg)
{
    msg=decryptXOR(msg,session_key);
    msg=msg+"1";
    return msg;
}
};


class BOB
{    public:
    string name;
    string alice;
    string random2;
    string session_key;
    string shared_key2;

    BOB(string name)
    {
        this->name=name;
    }

    void receiveMessage(string msg3,string msg4)
    {
      this->alice=decryptXOR(msg3,shared_key2);
      this->session_key=decryptXOR(msg4,shared_key2);
    }

    string sendMsgToAlice()
    {
      random2=generateNonce();
      return encryptXOR(random2,session_key);
    }

    bool verify(string res)
    {
        string msg=random2+"1";
        if(msg==res)
        return true;
        else
        return false;
    }
};

int main()
{ 
    string filename = "needham.txt";
    cout << endl;
    cout << "Welcome to the Login System" << std::endl;
    cout << endl;
    string username, password;
    cout << "Do you want to Register to KDC? (yes/no): ";
    string createNewUser;
    cin >> createNewUser;

    if (createNewUser == "yes")
    {
        cout << "Enter a new username: ";
        cin >> username;
        cout << "Enter a shared key ";
        cin >> password;
        cout << endl;
        storeUserPassword(filename, username, password);
        cout << "User successfully registered ";
    }
    else
    {
        string bob;
        string random1;
        string alice;

        cout << "Enter the Alice name";
        cin >> alice;
        cout << "Enter the bob name";
        cin >> bob;
        random1 = generateNonce();
        Alice a(bob, random1, alice);
        BOB b(bob);
        if(!a.sendRequestToKDC(filename,a.shared_key1,b.shared_key2))
        {
           cout<<"Not possible to Process Error Occured"<<endl;
           return 1;
        }
        else{
         string msg1=a.k.Msg[0];
         msg1=decryptXOR(msg1,a.shared_key1);
         string msg2=a.k.Msg[1];
         msg2=decryptXOR(msg2,a.shared_key1);
         
         if(a.bob!= msg2 || a.random1!= msg1)
         {
            cout<<"Not possible to Process Error Occured"<<endl;
           return 1;
         }
        a.session_key=decryptXOR(a.k.Msg[2],a.shared_key1);
        string msg3=a.k.Msg[3];
        msg3=decryptXOR(msg3,a.shared_key1);
        string msg4=a.k.Msg[4];
        msg4=decryptXOR(msg4,a.shared_key1);
        b.receiveMessage(msg3,msg4);

        string msgg=b.sendMsgToAlice();
        
        string  response=a.sendResponseToBob(msgg);
        
        if(b.verify(response))
        {
            cout<<"The Protocol working awesome"<<endl;
        }
         }
        }
     return 0;
}
