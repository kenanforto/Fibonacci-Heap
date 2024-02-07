#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
using namespace std;

struct FHNode {
    FHNode *parent;
    FHNode *child;
    FHNode *left;
    FHNode *right;
    int degree; 
    bool marked;
    int key;
};

class FibonacciHeap {

    private:
    FHNode *min;
    int nodes = 0;

    public:
     FibonacciHeap() {
         min = nullptr;
     }

    int getNodes() { return nodes; }
    int getMinKey() { return min->key; }
    FHNode *getMin() { return min; }

    //INSERT
     void fibHeapInsert(int key) {
         
         //kreiramo novi cvor
         FHNode *newNode = new FHNode(); //new FBNode
         newNode->key = key;
         newNode->degree = 0;
         newNode->marked = false;
         newNode->child = nullptr;

        //ako je gomila prazna,cvor se povezuje sa samim sobom, ciklicki
         if(min == nullptr) {
             min = newNode;
             newNode->left = newNode;
             newNode->right = newNode;
         }
         else {
             newNode->right = min; //desni pok novog cvora na min cvor
             newNode->left = min->left; //lijevi pok novog cvora na lijevog susjeda min
             min->left->right = newNode; //desni susjed lijevog susjeda min sa novim cvorom
             min->left = newNode; //lijevi pok min na novi cvor

             if(min->key > key) {
                 min = newNode;
             }
         }

          nodes++;
     }

     //UNION
     void fibHeapUnion(FibonacciHeap &H2) {
         //Ako je H prazan, a H2 nije, postavi H na H2
         if(min == nullptr || (H2.min != nullptr && H2.min->key < min->key)) {
             min = H2.min;
         }

         //Spoji root liste H i H2
         //unija radi tako sto root element H spoji sa prvim el H2,te se ustanovi novi minimum
         if (H2.min != nullptr) {
             if(min != nullptr) {
                 FHNode *temp = min->right;     //Sacuvaj desni cvor H
                 min->right = H2.min;          //Povezi desni cvor H s minimumom H2   
                 H2.min->left->right = temp; //Povezi desni cvor H2 s sacuvanim cvorom H
                 temp->left = H2.min->left; //Povezi sacuvani cvor H s lijevim cvorom H2
                 H2.min->left = min;      //Povezi lijevi cvor H2 s minimumom H
             }
             else {
                 min = H2.min;
             }
         }
        
         nodes += H2.nodes;
     }

     //EXTRACT MIN
     FHNode *fibHeapExtractMin() {
         FHNode *extractedMin = min;

         if(extractedMin != nullptr) {
             FHNode *child = extractedMin->child;
             while(child != nullptr) {
                 //spoji djecu sa root list
                 FHNode *nextChild = child->right;
                 min->left->right = child;
                 child->right = min;
                 child->left = min->left;
                 min->left = child;

                 child->parent = nullptr; //postavljanje roditelja na nullptr, jer nece imat vise djece
                 child = nextChild;
             }
             //ako je on jedini cvor, izbaci ga, te imamo praznu gomilu
             if(extractedMin == extractedMin->right) {
                 min = nullptr;
             }
             else {
                 //izbaci minimum iz gomile
                 extractedMin->left->right = extractedMin->right;
                 extractedMin->right->left = extractedMin->left;
                 min = extractedMin->right; //privremeno stavljamo min na desnog susjeda
                 consolidate();
             }

             nodes--;
         }
         return extractedMin;
     }
void ispisiHeap()
{
    FHNode* x=min;
    do {
        FHNode* nextNode=x->right;
        std::cout<<" ("<<x->key<<")";
        x=nextNode;
    }while (x!=min);
}
void consolidate() {

    int D = static_cast<int>(log2(nodes))+1; //Fibonacijeva konstanta φ (phi) koja je priblizno 1.618
    vector<FHNode*> degrees(D, nullptr);
    FHNode *x = min;

    for(;;){
        int degree = x->degree;
        FHNode *sljedeci = x->right;

        while(degrees[degree] != nullptr) {

            FHNode *y = degrees[degree];
            if(x->key > y->key) {
                FHNode *b = x;
                x=y;
                y=b;
            }
            if (y==min) {
                min = x;
            }
            fibHeapLink(y,x);
            if(x->right == x) {
                min=x;
            }
            degrees[degree] = nullptr;
            degree++;
        }
        degrees[degree] = x;
        x = sljedeci;
        if (x == min){
            break;
        }
    }
        min = nullptr;
        for(int i = 0; i < D;i++) {
            if(degrees[i] != nullptr) {
                if(min==nullptr) {
                    min=degrees[i];
                }
                else{
                    degrees[i]->right = min;
                    degrees[i]->left = min->left;
                    min->left->right = degrees[i];
                    min->left = degrees[i];
                    if(degrees[i]->key < min->key) {
                        min = degrees[i];
                    }
                }
            }
        }
}

//dodjeljuje jedan cvor drugom ako im je isti degree (uvijek onaj sa vecim kljucem postane dijete)
void fibHeapLink(FHNode *child, FHNode *parent) {
    child->left->right = child->right;
    child->right->left = child->left;

    child->parent = parent;
    if(parent->child == nullptr) {
        parent->child = child;
        child->left = child->right = child;
    }
    else {
        child->left = parent->child;
        child->right = parent->child->right;
        parent->child->right->left = child;
        parent->child->right=child;
    }
    parent->degree++;
    child->marked = false;
}

//smanji vrijednost kljuca
void fibHeapDecreaseKey(FHNode *x, int k){
   if (k > x->key) throw std::domain_error("Novi kljuc je veci od trenutnog kljuca!");

   x->key = k;
   FHNode *y = x->parent;

   if (y != nullptr && x->key < y->key) {
    cut(x, y);
    cascadingCut(y);
   }

   if (x->key < min->key) {
       min = x;
   }
}

//cilj je odsjeci cvor x od roditelja y
void cut(FHNode *x,FHNode *y) {
    //ako je x jedino dijete cvora
    if(x == x->right) {
        y->child = nullptr;
    }
    else {
        //moramo izbrisati x iz liste djece y;Postavljamo pokazivace da "preskoce" cvor x u listi djece y.
        x->left->right = x->right;
        x->right->left = x->left;
        if(y->child == x) { //Ako je x prvo dijete y, tada pok child y pokazuje na desnog susjeda cvora x
            y->child = x->right;
        }
    }

    //povezuje se x sa root
    x->left = min;
    x->right = min->right;
    min->right->left = x;
    min->right = x;

    x->parent = nullptr;
    x->marked = false;
}

//izvrsava se kada je roditelj marked
void cascadingCut(FHNode *y) {
    FHNode *z = y->parent;
    if(z != nullptr) { //kada je z nullptr, dosli smo do rootlist
        if(!y->marked) {
            y->marked = true;
        }
        else {
            //odsjecanje cvora y od roditelja z
            cut(y,z);
            cascadingCut(z);
        }
    }
}

 //DELETE
 void fibHeapDelete(FHNode *x) {
     fibHeapDecreaseKey(x, std::numeric_limits<int>::min()); //vraca najmanju vr koja moze biti smjestena u int
     fibHeapExtractMin();
 }

};

void fibHeapInsertTest() {
     cout<< "--------------------INSERT---------------------"<<endl;
    FibonacciHeap FH;
    FH.fibHeapInsert(3);
    if (FH.getNodes() == 1 && FH.getMinKey() == 3) {
        cout<< "Element 3 je USPJESNO dodan u gomilu! Broj elemenata je "<< FH.getNodes() << ", a minimum je "<< FH.getMin()->key<<endl;
    }
    else cout<< "Element 3 NIJE dodan u gomilu!";

    FH.fibHeapInsert(11);
        if (FH.getNodes() == 2 && FH.getMinKey() == 3) {
        cout<< "Element 11 je USPJESNO dodan u gomilu! Broj elemenata je "<< FH.getNodes() << ", a minimum je "<< FH.getMin()->key<<endl;
    }
    else cout<< "Element 11 NIJE dodan u gomilu!";
    
    FH.fibHeapInsert(2);
    if (FH.getNodes() == 3 && FH.getMinKey() == 2) {
        cout<< "Element 2 je USPJESNO dodan u gomilu! Broj elemenata je "<< FH.getNodes() << ", a minimum je "<< FH.getMin()->key<<endl;
    }
    else cout<< "Element 2 NIJE dodan u gomilu!";
   // FH.ispisiHeap();
}

void fibHeapUnionTest() {
    cout<< "\n--------------------UNION----------------------"<<endl;
    FibonacciHeap FH, FH1;

    //UNIJA PRAZNIH GOMILA
    FH.fibHeapUnion(FH1);
    if (!FH.getNodes() && !FH.getMin()) {
        cout << "USPJESNA unija praznih gomila!"<<endl;
    }
    else cout << "NEUSPJESNA unija praznih gomila!"<<endl;

    //UNIJA PRAZNE I NEPRAZNE GOMILE
    FH1.fibHeapInsert(4);
    FH1.fibHeapInsert(1);
    FH.fibHeapUnion(FH1);
    if(FH.getNodes() == 2 && FH.getMinKey() == 1) {
        cout<< "USPJESNA unija prazne i neprazne gomile!"<<endl;
    }
    else cout<< "NEUSPJESNA unija prazne i neprazne gomile!"<<endl;

    //UNIJA NEPRAZNIH GOMILA, FH1 i FH imaju vec po 2 elementa, linija 133
    FH1.fibHeapInsert(5);
    FH.fibHeapInsert(7);
    FH.fibHeapUnion(FH1); 
    if(FH.getNodes() == 6 && FH.getMinKey() == 1) {
        cout<< "USPJESNA unija nepraznih gomila!"<<endl;
    }
    else cout<< "NEUSPJESNA unija nepraznih gomila!"<<endl;
}

void fibHeapExtractMinTest() {
    cout<< "\n--------------------EXTRACT MIN----------------"<<endl;
    //PRAZNA GOMILA
    FibonacciHeap FH;
    FHNode *izbaceni = FH.fibHeapExtractMin();
    if (!FH.getNodes() && !FH.getMin() && !izbaceni) {
        cout << "USPJESNO izbacen minimalni cvor iz prazne gomile!"<<endl;
    }
    else cout << "NEUSPJESNO izbacen minimalni cvor iz prazne gomile!"<<endl;

    //GOMILA SA JEDNIM CVOROM
    FH.fibHeapInsert(6);
    izbaceni = FH.fibHeapExtractMin();
    if (!FH.getNodes() && !FH.getMin() && izbaceni->key == 6) {
        cout << "USPJESNO izbacen minimalni cvor iz gomile sa jednim cvorom!"<<endl;
    }
    else cout << "NEUSPJESNO izbacen minimalni cvor iz gomile sa jednim cvorom!"<<endl;

    //GOMILA SA 2 CVORA
    FH.fibHeapInsert(6);
    FH.fibHeapInsert(2);
    izbaceni = FH.fibHeapExtractMin();
    if (FH.getNodes() == 1 && FH.getMinKey() == 6 && izbaceni->key == 2) {
        cout << "USPJESNO izbacen minimalni cvor iz gomile sa dva cvora! Kljuc izbacenog cvora iznosi "<<izbaceni->key<<endl;
    }
    else cout << "NEUSPJESNO izbacen minimalni cvor iz gomile sa dva cvora!"<<endl;

    //GOMILA SA VISE CVOROVA
    FH.fibHeapInsert(10);
    FH.fibHeapInsert(1);
    FH.fibHeapInsert(7);
    izbaceni = FH.fibHeapExtractMin();
    if (FH.getNodes() == 3 && FH.getMinKey() == 6 && izbaceni->key == 1) {
        cout << "USPJESNO izbacen minimalni cvor iz gomile sa vise cvorova! Kljuc izbacenog cvora iznosi "<<izbaceni->key<<endl;
    }
    else cout << "NEUSPJESNO izbacen minimalni cvor iz gomile sa vise cvorova!"<<endl;
}


void fibHeapDecreaseKeyTest() {
    cout<< "\n--------------------DECREASE KEY----------------"<<endl;
    // INSERT VECI BROJ OD TRENUTNOG KLJUCA
    FibonacciHeap FH;
    FH.fibHeapInsert(6);
    try {
        FH.fibHeapDecreaseKey(FH.getMin(), 10);
        cout<<"NEUSPJESNO zaustavljeno smanjivanje kljuca vecim brojem"<<endl;
    }
    catch(...) {
        cout<<"USPJESNO zaustavljeno smanjivanje kljuca vecim brojem"<<endl;
    }
    //INSERTT MANJI BROJ OD TRENUTNOG KLJUCA
    FH.fibHeapDecreaseKey(FH.getMin(), 1);
     if (FH.getNodes() == 1 && FH.getMinKey() == 1) {
            cout<<"USPJESNO smanjen minimalni cvor. Kljuc iznosi "<<FH.getMinKey()<<endl;
     }
     else {
            cout<<"NEUSPJESNO smanjen minimalni cvor"<<endl;
     }
    //PROMIJENI KLJUC DJETETA vecim brojem od minimalnog
    FH.fibHeapInsert(10);
    FH.fibHeapInsert(20);
    FH.fibHeapExtractMin();
    FH.fibHeapDecreaseKey(FH.getMin()->child, 12);
    if(FH.getNodes() == 2 && FH.getMinKey() == 10 && FH.getMin()->child->key == 12) {
        cout<<"USPJESNO smanjivanje djeteta vecim od minimalnog!"<<endl;
    }
    else {
        cout<<"NEUSPJESNO smanjivanje djeteta vecim brojem od minimalnog!"<<endl;
    }
    //PROMIJENI KLJUC DJETETA manjim brojem od minimalnog
    FH.fibHeapDecreaseKey(FH.getMin()->child, 6);
    if(FH.getNodes() == 2 && FH.getMinKey() == 6) {
        cout<<"USPJESNO smanjivanje djeteta manjim brojem od minimalnog!"<<endl;
    }
    else {
        cout<<"NEUSPJESNO smanjivanje djeteta manjim brojem od minimalnog!"<<endl;
     }
}

void fibHeapDeleteTest(){
    cout<< "\n--------------------DELETE----------------"<<endl;
    FibonacciHeap FH;
    FH.fibHeapInsert(5);
    FH.fibHeapDelete(FH.getMin());
    if(!FH.getNodes() && !FH.getMin()) {
        cout<<"USPJESNO obrisan min cvor!"<<endl;
    }
    else {
        cout<<"NESUPJESNO obrisan min cvor!";
    }
    FH.fibHeapInsert(5);
    FH.fibHeapInsert(7);
    FH.fibHeapInsert(3);
    FH.fibHeapExtractMin();
    FH.fibHeapDelete(FH.getMin()->child);
    if(FH.getNodes() == 1 && FH.getMinKey() == 5) {
        cout<<"USPJESNO obrisani cvorovi!"<<endl;
    }
    else {
        cout<<"NESUPJESNO obrisani cvorovi!";
    }
}


int main() {
 fibHeapInsertTest();
 fibHeapUnionTest();
 fibHeapExtractMinTest();
 fibHeapDecreaseKeyTest();
 fibHeapDeleteTest();
}
