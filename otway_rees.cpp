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
        ciphertext[i] = plaintext[i] ^ key[i % key.size()];
    }
    return ciphertext;
}

string decryptXOR(const string &ciphertext, const string &key)
{
    string plaintext = ciphertext;
    for (size_t i = 0; i < ciphertext.size(); ++i)
    {
        plaintext[i] = ciphertext[i] ^ key[i % key.size()];
    }
    return plaintext;
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


bool check( map<string, string> userPasswords,string alice,string &ka)
{
    auto it2=userPasswords.find(alice);
    if (it2 != userPasswords.end()) {
      ka=it2->second;
        return true;
    }
    else
    return false;
}


class KDC
{

public:
    

    string Msg[5];
    

    void generateSessionkey(string message[8],string alice,string bob,string common_random)
    {       string filename="Otway_rees.txt";
    string ka="";
    string kb="";
            map<string, std::string> userPasswords = readUserPasswordsFromFile(filename);
         if(check(userPasswords,alice,ka))
         {
            if(check(userPasswords,bob,kb));
         }

          
          string session_key=generateNonce();
          Msg[0]=common_random;
          Msg[1]=encryptXOR(decryptXOR(message[7],kb),kb);
          Msg[2] = encryptXOR(session_key, kb);
          Msg[3]=encryptXOR(decryptXOR(message[3],ka),ka);
          Msg[4] = encryptXOR(session_key, ka);
    }
};







class Alice
{
public:
    string name;
    string bob;
    string common_random;
    string random1;
    string session_key;
    string message[7];
    string shared_key1;
    
    
    Alice(string bob, string random1, string alice,string common_random)
    {
        this->name = alice;
        this->bob = bob;
        this->random1 = random1;
        this->common_random = common_random;
    }


    bool sendMessageToBob(string filename,string &ka)
    {
        message[0]=name;
        message[1]=bob;
        message[2]=common_random;
        map<string, std::string> userPasswords = readUserPasswordsFromFile(filename);
        if(check(userPasswords,name,ka))
        {
            shared_key1=ka;
        message[3]= encryptXOR(name,ka);
        message[4]=encryptXOR(bob,ka);
        message[5]=encryptXOR(common_random,ka);
        message[6]= encryptXOR(random1,ka);
        return true;
        } 
        else
        return false;
    }

    bool verify(string res)
    {
        res=decryptXOR(res,shared_key1);
        if(res==random1)
        {
            return true;
        }
        else
        return false;
    }

};



class BOB
{    public:
    string name;
    string alice;
    string common_random;
    string random2;
    string session_key;
    string shared_key2;
    string message[8];
    KDC k;

    void receiveMessage(string msg1,string msg2,string msg3)
    {
      this->alice=msg1;
      this->name=msg2;
      this->common_random=msg3;
      random2=generateNonce();
    }

    bool sendMsgToKDC(Alice a,string filename)
    {
           string kb="";
      map<string, std::string> userPasswords = readUserPasswordsFromFile(filename);
        if(check(userPasswords,name,kb))
        { shared_key2=kb;
        message[0]=a.message[3];
      message[1]=a.message[4];
      message[2]=a.message[5];
      message[3]=a.message[6];
      message[4]=encryptXOR(alice,shared_key2);
      message[5]=encryptXOR(name,shared_key2);
      message[6]=encryptXOR(common_random,shared_key2);
      message[7]=encryptXOR(random2,shared_key2);

      k.generateSessionkey(message,a.name,a.bob,common_random);

        return true;
        } 
        else
        return false;
    }



     string sendResponse_to_Alice(Alice &a)
     {
        a.session_key=decryptXOR(k.Msg[4],a.shared_key1);
        string p=decryptXOR(k.Msg[3],a.shared_key1);
        return p;
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
    string filename = "Otway_rees.txt"; // Ensure the correct filename is used
    cout << "Welcome to the Login System" << std::endl;
    string username, password;
    cout << "Do you want to Register to KDC? (yes/no): ";
    string createNewUser;
    cin >> createNewUser;

    if (createNewUser == "yes")
    {
        cout << "Enter a new username: ";
        cin >> username;
        cout << "Enter a shared key: ";
        cin >> password;
        cout << "User successfully registered." << endl;
        storeUserPassword(filename, username, password);
    }
    else
    {
        string bob;
        string common_random;
        string random1;
        string alice;

        cout << "Enter the Alice name: ";
        cin >> alice;
        cout << "Enter the bob name: ";
        cin >> bob;
        random1 = generateNonce();
        common_random = generateNonce();

        Alice a(bob, random1, alice, common_random);
        BOB b;

       
        string ka;
        if (!a.sendMessageToBob(filename, ka))
        {
            cout << "Not possible to Process Error Occurred in sendMessageToBob" << endl;
            return 1;
        }

          cout << "Sending the ticket of the alice to the bob" << endl;

        // Debugging output for Bob's received message
        string msg1 = a.message[0];
        string msg2 = a.message[1];
        string msg3 = a.message[2];
        b.receiveMessage(msg1, msg2, msg3);

        // Debugging output for Bob's message to KDC
        if (!b.sendMsgToKDC(a, filename))
        {
            cout << "Not possible to Process Error Occurred in sendMsgToKDC" << endl;
            return 1;
        }
         cout << "Bob sending the tickets to the KDC" << endl;
     
        b.session_key=decryptXOR(b.k.Msg[2],b.shared_key2);
        cout<<"Bob received the session key"<<endl;
        string res=b.sendResponse_to_Alice(a);
        if(res==a.random1)
        {
            cout<<"Alice received the session key"<<endl;
        }
        else
        cout<<"Need to work";
    }

    return 0;
}