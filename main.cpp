// Actividad 3.4 - Actividad Integral de Heaps
//
// Lee la bitacora ordenada por ip destino que genero la actividad integral
// anterior, cuenta cuantas veces aparece cada ip destino y guarda cada par
// (ip, numero de accesos) en un vector. Convierte el vector en un heap en el que la
// prioridad es el numero de accesos y saca las N ip destino con mas
// accesos que indique el usuario.
//
// equipo 1
// Alan Farid Hernandez Sanmartin - A01753660
// Alejandro Sanchez Calderon - A01754913
// Fecha: 23-11-2026
// compilar con: cmake -B build && cmake --build build && ./build/actividad_integral_3
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>

using namespace std;
//Bitacora que salio de la actividad anterior ordenada por ip y
// destino
const string INPUT_FILE = "bitacora_ordenada_ip.csv";
//El Numero de campos que conforman un evento de la bitacora
const int FIELD_COUNT = 8;

// Struct representing an IP address
struct ip {
    int o1;
    int o2;
    int o3;
    int o4;
};

// Struct representing a log event (connection between two nodes)
// ts: Date-time of the event as ctime's tm (check docs: https://cplusplus.com/reference/ctime/tm/)
// ip_o: IP of the origin node
// port_o: Port of the origin node
// domain_o: Domain of the origin node
// ip_d: IP of the destination node
// port_d: Port of the destination node
// domain_d: Domain of the destination node
struct event {
    struct tm ts;
    struct ip ip_o;
    string port_o;
    string domain_o;
    struct ip ip_d;
    string port_d;
    string domain_d;
};

// Escribe una direccion ip en un flujo de salida con el formato de la
// bitacora.Una ip cuyo primer octeto es 0 representa un valor que no aplica
// y se imprime como el caracter '-'.
// Los parametros:
//   os: flujo de salida donde se escribira la direccion.
//   address: direccion ip a escribir.
// Retorna:
// El mismo flujo de salida, para permitir encadenar operaciones.
ostream& operator<<(ostream &os, const ip &address) {
    if (address.o1 == 0) {
        os << "-";
    } else {
        os << address.o1 << "." << address.o2 << "." << address.o3 << "."
           << address.o4;}
    return os;}

// Escribe un evento en un flujo de salida usando el mismo formato de la
// bitacora.
//Los parametros:
//   os: flujo de salida donde se escribira el evento.
//   e: evento a escribir.
// Retorno:
//   el mismo flujo de salida, para permitir encadenar operaciones.
ostream& operator<<(ostream &os, const event &e) {
    // usamos %H:%M:%S en lugar de %T porque la biblioteca de C de Windows que
    // usa MinGW no reconocia %T:strftime falla y deja la cadena vacia, y el
    // evento se imprimia sin fecha ni hora
    char dateOutput[20] = ""; strftime(dateOutput, 20, "%d-%m-%Y,%H:%M:%S", &e.ts);
    os << dateOutput << "," << e.ip_o << "," << e.port_o << ","
       << e.domain_o << "," << e.ip_d << "," << e.port_d << ","
       << e.domain_d;
    return os;}

// Escribe un par (ip, numero de accesos) en un flujo de salida con el formato de
//"ip: accesos".
//Los parametros:
//   os: flujo de salida donde se escribira el par.
//   p: par a escribir.
// Retorno:
// El mismo flujo de salida, para permitir encadenar operaciones.
ostream& operator<<(ostream &os, const pair<ip, int> &p) {
    os << p.first << ": " << p.second;
    return os;
}

// Determina si una direccion ip es menor que otra. Compara los octetos del
// mas significativo al menos significativo: en cuanto encuentra un octeto
// distinto, ese octeto decide el orden; solo cuando hay empate se revisa el
// siguiente.
// Los parametros:
//   ip1: primera direccion a comparar.
//   ip2: segunda direccion a comparar.
// Retorno:
//   true si ip1 es menor que ip2, false en cualquier otro caso.
bool operator<(const ip &ip1, const ip &ip2) {
    if (ip1.o1 != ip2.o1) {
        return ip1.o1 < ip2.o1;
    }
    if (ip1.o2 != ip2.o2) {
        return ip1.o2 < ip2.o2;
    }
    if (ip1.o3 != ip2.o3) {
        return ip1.o3 < ip2.o3;
    }
    return ip1.o4 < ip2.o4;
}

// Determina si dos direcciones ip son iguales.
// Los parametros:
// ip1: primera direccion a comparar.
// ip2: segunda direccion a comparar.
// Retorno:
// true si los cuatro octetos coinciden, false en cualquier otro caso.
bool operator==(const ip &ip1, const ip &ip2) {
    return ip1.o1 == ip2.o1 && ip1.o2 == ip2.o2 && ip1.o3 == ip2.o3
           && ip1.o4 == ip2.o4;}

// Determina la prioridad relativa de dos pares (ip, numero de accesos). Es el
// criterio que usan make_heap y pop_heap:el par que esmayor queda al frente del
//heap, por lo que tiene mas prioridsd el par con mas accesos.
//Cuando dos pares tienen los mismos accesos, tiene mas prioridad el de ip
// menor. Sin este desempate, el orden entre ip empatadas dependeria de como
// make_heap acomode internamente el vector, y la salida podria cambiar.
//Los parametros:
//   p1: es el primer par a comparar.
//   p2: es el segundo par a comparar.
// Retorno:
//   true si p1 tiene menos prioridad que p2, false en cualquier otro caso.
bool operator<(const pair<ip, int> &p1, const pair<ip, int> &p2) {
    if (p1.second != p2.second) {return p1.second < p2.second;}
    return p2.first < p1.first;
}
// Convierte una cadena con formato "a.b.c.d" en una estructura ip.
// Los parametros:
//   text: cadena con la direccion ip, o "-" cuando el valor no aplica.
//Retorno:
// La direccion ip con sus cuatro octetos. Cuando text es "-" o esta vacia
// regresa los cuatro octetos en cero, valor que el operador << imprime de
// vuelta como '-'.
ip parseIp(const string &text) {
    ip result = {0, 0, 0, 0};
    if (text == "-" || text.empty()) {
        return result;}
    istringstream stream(text);
    char dot = ' ';
    stream >> result.o1 >> dot >> result.o2 >> dot >> result.o3 >> dot >> result.o4;
    return result;}
// Convierte las cadenas de fecha y hora de la bitacora en una estructura tm.
// Los parametros:
//   date: es la cadena con la fecha en formato "dd-mm-aaaa".
//   time: es la cadena con la hora en formato "hh:mm:ss".
// Retorno:
// La fecha y hora correspondientes como estructura tm.
tm parseTimestamp(const string &date, const string &time) {
    tm timestamp = {};
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    char separator = ' ';

    istringstream dateStream(date);
    dateStream >> day >> separator >> month >> separator >> year;
    istringstream timeStream(time);
    timeStream >> hour >> separator >> minute >> separator >> second;
    timestamp.tm_year = year - 1900;
    timestamp.tm_mon = month - 1;
    timestamp.tm_mday = day;
    timestamp.tm_hour = hour;
    timestamp.tm_min = minute;
    timestamp.tm_sec = second;
    timestamp.tm_isdst = -1;
    return timestamp;}

// Convierte una linea de la bitacora en un evento. El formato que deberia salir es
// fecha,hora,ip origen,puerto origen,dominio origen,ip destino,puerto destino,
// dominio destino.
// Los parametros:
//  line: linea de texto a procesar.
//  e: evento donde se guardaran los valores obtenidos.
//Retorno:
//   true si la linea contenia los ocho campos y se pudo procesar,false si el
//   formato era invalido.
bool parseEvent(const string &line, event &e) {
    istringstream stream(line);
    string fields[FIELD_COUNT] = {};
    for (int i = 0; i < FIELD_COUNT; i++) {
        if (!getline(stream, fields[i], ',')) {
            return false;}
    }
    // Los archivos con saltos de linea de Windows dejan un retorno de carro al
    // final del ultimo campo, que debe eliminarse para no contaminar el dominio
    if(!fields[7].empty() && fields[7].back() == '\r') {
        fields[7].pop_back();
    }
    e.ts = parseTimestamp(fields[0], fields[1]);
    e.ip_o = parseIp(fields[2]);
    e.port_o = fields[3];
    e.domain_o = fields[4];
    e.ip_d = parseIp(fields[5]);
    e.port_d = fields[6];
    e.domain_d = fields[7];
    return true;
}

// Lee la bitacora y cuenta cuantas veces aparece cada ip destino. Como la
// bitacora esta ordenada por ip destino, los eventos de una misma ip son
// consecutivos: basta comparar cada evento con el ultimo par agregado al
// vector. Si la ip coincide se incrmenta su contador; si no, empieza una ip
// nueva. Asi el conteo se hace en un solo recorrido del archivo.
// Los parametros:
//   filename: es el nombre del archivo de bitacora a leer.
//   counts: es el vector donde se guardara un par (ip, accesos) por ip destino.
// Retorno:
//   true si el archivo pudo abrirse, false en caso contrario.
bool countAccesses(const string &filename, vector<pair<ip, int>> &counts) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: no se pudo abrir el archivo '" << filename << "'\n";
        return false;}

    string line = "";
    while (getline(file, line)) {
        event e = {};
        if (!line.empty() && parseEvent(line, e)) {
            if (!counts.empty() && counts.back().first == e.ip_d) {
                counts.back().second++;
            } else {
                counts.push_back(pair<ip, int>{e.ip_d, 1});}
        }
    }

    file.close();
    return true;
}
// Solicita al usuario por consola cuantas ip quiere desplegar y valida que sea
// un numero entero positivo.
// Los parametros:
//   n: variable donde se guardara el valor capturado.
// Retorno:
//   true si el valor capturado es valido, false en caso contrario.
bool readCount(int &n) {
    cout << "\nCuantas ip destino con mas accesos deseas ver? ";
    cin >> n;
    return !cin.fail() && n > 0;
}

// Despliega las n ip con mas accesos extrayendolas del heap una por una. El
// elemento de mayor prioridad siempre esta en el frente del vector; pop_heap
// lo mueve al final y reacomoda el resto como heap, y pop_back lo elimina.
// Al terminar, el vector conserva solo las ip que no se desplegaron.
// Los parametros:
//   heap: vector previamente convertido en heap con make_heap.
//   n: numero de ip a desplegar; debe ser menor o igual al tamano del heap.
void printTop(vector<pair<ip, int>> &heap, int n) {
    for (int i = 1; i <= n; i++) {
        cout << i << ". " << heap.front() << "\n";
        pop_heap(heap.begin(), heap.end());
        heap.pop_back();}}

// Programa principal. Cuenta los accesos a cada ip destino, convierte los
// conteos en un heap y despliega las N ip con mas accesos.
// Retorno:
//   0 si la ejecucion fue exitosa, 1 si ocurrio un error de lectura o de
//   captura de datos.
int main() {
    vector<pair<ip, int>> v{};

    cout << "Leyendo bitacora '" << INPUT_FILE << "'...\n";
    if (!countAccesses(INPUT_FILE, v) || v.empty()) {
        cout << "No se registraron eventos. Verifica el nombre del archivo.\n";
        return 1;
    }
    cout << "Ip destino distintas: " << v.size() << "\n";

    //mke_heap usa el operador < sobrecargado para pair<ip, int>: deja al
    //frente del vector el par con mas accesos
    make_heap(v.begin(), v.end());
    int n = 0;
    if (!readCount(n)) {
        cout << "Valor invalido. Debe ser un numero entero mayor que 0.\n";
        return 1;
    }
    int total = v.size();
    if (n > total) {
        cout << "Solo hay " << total << " ip destino; se despliegan todas.\n";
        n = total;}

    cout << "\nIp destino con mas accesos (" << n << "):\n";
    printTop(v, n);
    return 0;}