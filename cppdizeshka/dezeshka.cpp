#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

using namespace std;
class asset
{
    const string put_;

public:
    explicit asset(const string& put) : put_(put) {}

    virtual char* prochitat() = 0;
    virtual ~asset() = default;

    const string& poluchit_put() const {
        return put_;
    }
};
class tekstoviy_asset : public asset
{
    string tekst_;

public:
    explicit tekstoviy_asset(const string& put) : asset(put)
    {
        cout << "[sistema] zagruzka fayla s diska: "
            << put << endl;

        ifstream fayl(put);

        if (fayl.is_open()) {
            stringstream bufer;
            bufer << fayl.rdbuf();
            tekst_ = bufer.str();
        }
        else {
            cerr << "[oshibka] ne udalos otkrit fayl: "
                << put << endl;

            tekst_ = "[fayl pustoy ili ne nayden]";
        }
    }

    ~tekstoviy_asset() override
    {
        cout << "[sistema] vygruzka asseta iz pamyati: "
            << poluchit_put() << endl;
    }

    char* prochitat() override
    {
        return tekst_.data();
    }
};
class menedzher_assetov
{
    map<string, weak_ptr<asset>> assety_;

public:
    shared_ptr<asset> poluchit(const string& put)
    {
        auto it = assety_.find(put);

        if (it != assety_.end()) {
            if (auto ukazatel = it->second.lock()) {
                cout << "[sistema] asset nayden v keshe: "
                    << put << endl;

                return ukazatel;
            }
            else {
                assety_.erase(it);
            }
        }

        auto ukazatel = make_shared<tekstoviy_asset>(put);
        assety_[put] = ukazatel;

        return ukazatel;
    }
};

void pokazat_menu()
{
    cout << "\n=== MENYU UPRAVLENIYA ASSETAMI ===" << endl;
    cout << "1. Zagruzit asset" << endl;
    cout << "2. Pokazat otkrytye assety" << endl;
    cout << "3. Prochitat asset" << endl;
    cout << "4. Vygruzit asset" << endl;
    cout << "5. Vyhod" << endl;
    cout << "Vvedite nomer: ";
}

int main()
{
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    menedzher_assetov menedzher;
    vector<shared_ptr<asset>> otkrytye_assety;

    int vibor = 0;

    do {
        pokazat_menu();

        if (!(cin >> vibor)) {
            cout << "Oshibka vvoda. Poprobuyte eshe raz." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (vibor) {
        case 1: {
            string put;

            cout << "Vvedite put k faylu (naprimer, test.txt): ";
            cin >> put;

            auto ukazatel = menedzher.poluchit(put);
            otkrytye_assety.push_back(ukazatel);

            cout << "Asset zagruzhen." << endl;
            break;
        }

        case 2: {
            if (otkrytye_assety.empty()) {
                cout << "Spisok assetov pust." << endl;
            }
            else {
                cout << "\nOtkrytye assety" << endl;

                for (size_t i = 0; i < otkrytye_assety.size(); ++i) {
                    cout << i + 1 << ". "
                        << otkrytye_assety[i]->poluchit_put()
                        << endl;
                }
            }
            break;
        }

        case 3: {
            if (otkrytye_assety.empty()) {
                cout << "Net assetov dlya chteniya." << endl;
                break;
            }

            cout << "\n--- Otkrytye assety ---" << endl;

            for (size_t i = 0; i < otkrytye_assety.size(); ++i) {
                cout << i + 1 << ". "
                    << otkrytye_assety[i]->poluchit_put()
                    << endl;
            }

            cout << "Vvedite nomer asseta: ";

            size_t nomer;
            cin >> nomer;

            if (nomer > 0 && nomer <= otkrytye_assety.size()) {
                cout << "\n--- Soderzhimoe fayla ---" << endl;

                cout << otkrytye_assety[nomer - 1]->prochitat()
                    << endl;

                cout << "------------------------" << endl;
            }
            else {
                cout << "Neverniy nomer asseta." << endl;
            }
            break;
        }

        case 4: {
            if (otkrytye_assety.empty()) {
                cout << "Net assetov dlya vygruzki." << endl;
                break;
            }

            cout << "\n--- Otkrytye assety ---" << endl;

            for (size_t i = 0; i < otkrytye_assety.size(); ++i) {
                cout << i + 1 << ". "
                    << otkrytye_assety[i]->poluchit_put()
                    << endl;
            }

            cout << "Vvedite nomer asseta dlya zakrytiya: ";

            size_t nomer;
            cin >> nomer;

            if (nomer > 0 && nomer <= otkrytye_assety.size()) {
                cout << "Zakryvaem asset: "
                    << otkrytye_assety[nomer - 1]->poluchit_put()
                    << endl;

                otkrytye_assety.erase(
                    otkrytye_assety.begin() + (nomer - 1)
                );
            }
            else {
                cout << "Neverniy nomer asseta." << endl;
            }
            break;
        }

        case 5:
            cout << "Vyhod iz programmy..." << endl;
            break;

        default:
            cout << "Neverniy vibor. Vvedite nomer ot 1 do 5."
                << endl;
        }

    } while (vibor != 5);

    return 0;
}