#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <cctype>

using namespace std;

// Sistema sencillo de restaurante con KDS (cocina) y OSS (consulta de pedidos).
// Variante B implementada provisionalmente: priorizacion de pedidos en cocina.
// Los pedidos pendientes aparecen primero y, dentro de cada estado, los mas antiguos.

const int MAX_PEDIDOS = 500;
const int TOTAL_PRODUCTOS = 6;
const string ARCHIVO = "pedidos.txt";

struct Producto {
    string nombre;
    double precio;
};

struct Pedido {
    int numero;
    string cliente;
    int producto;       // Posicion del producto en el catalogo
    int cantidad;
    string estado;
};

Producto menu[TOTAL_PRODUCTOS] = {
    {"Hamburguesa", 35.00},
    {"Pizza personal", 45.00},
    {"Hot dog", 22.00},
    {"Papas fritas", 18.00},
    {"Gaseosa", 10.00},
    {"Combo de pollo", 40.00}
};

Pedido pedidos[MAX_PEDIDOS];
int totalPedidos = 0;
int siguienteNumero = 1;

// Lee un entero validando que toda la entrada sea numerica y este en el rango.
int leerEntero(const string& mensaje, int minimo, int maximo) {
    string linea;
    int valor;
    while (true) {
        cout << mensaje;
        getline(cin, linea);
        stringstream ss(linea);
        char extra;
        if (ss >> valor && !(ss >> extra) && valor >= minimo && valor <= maximo) {
            return valor;
        }
        cout << "Entrada invalida. Ingrese un numero entre "
             << minimo << " y " << maximo << ".\n";
    }
}

string leerTexto(const string& mensaje) {
    string texto;
    do {
        cout << mensaje;
        getline(cin, texto);
        if (texto.empty()) cout << "El dato no puede estar vacio.\n";
    } while (texto.empty());
    // Evita romper el formato de almacenamiento separado por |
    replace(texto.begin(), texto.end(), '|', '/');
    return texto;
}

string minusculas(string texto) {
    transform(texto.begin(), texto.end(), texto.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return texto;
}

double subtotal(const Pedido& p) {
    return menu[p.producto].precio * p.cantidad;
}

void guardarPedidos() {
    ofstream archivo(ARCHIVO);
    if (!archivo) {
        cout << "No se pudo guardar el archivo " << ARCHIVO << ".\n";
        return;
    }

    for (int i = 0; i < totalPedidos; i++) {
        archivo << pedidos[i].numero << '|'
                << pedidos[i].cliente << '|'
                << pedidos[i].producto << '|'
                << pedidos[i].cantidad << '|'
                << pedidos[i].estado << '\n';
    }
}

void cargarPedidos() {
    ifstream archivo(ARCHIVO);
    if (!archivo) return; // Primera ejecucion: el archivo aun no existe.

    string linea;
    while (getline(archivo, linea) && totalPedidos < MAX_PEDIDOS) {
        stringstream ss(linea);
        string numero, cliente, producto, cantidad, estado;

        if (!getline(ss, numero, '|') ||
            !getline(ss, cliente, '|') ||
            !getline(ss, producto, '|') ||
            !getline(ss, cantidad, '|') ||
            !getline(ss, estado, '|')) {
            continue;
        }

        try {
            Pedido p;
            p.numero = stoi(numero);
            p.cliente = cliente;
            p.producto = stoi(producto);
            p.cantidad = stoi(cantidad);
            p.estado = estado;

            if (p.numero > 0 && p.producto >= 0 &&
                p.producto < TOTAL_PRODUCTOS && p.cantidad > 0 &&
                (p.estado == "Pendiente" || p.estado == "Preparando" ||
                 p.estado == "Listo" || p.estado == "Entregado" ||
                 p.estado == "Cancelado")) {

                bool duplicado = false;
                for (int i = 0; i < totalPedidos; i++) {
                    if (pedidos[i].numero == p.numero) duplicado = true;
                }
                if (!duplicado) {
                    pedidos[totalPedidos++] = p;
                    if (p.numero >= siguienteNumero) siguienteNumero = p.numero + 1;
                }
            }
        } catch (...) {
            // Ignora lineas dañadas o con datos no numericos.
        }
    }
}

void mostrarMenuProductos() {
    cout << "\n========== MENU DEL RESTAURANTE ==========\n";
    for (int i = 0; i < TOTAL_PRODUCTOS; i++) {
        cout << i + 1 << ". " << left << setw(20) << menu[i].nombre
             << "Q " << fixed << setprecision(2) << menu[i].precio << '\n';
    }
}

int buscarPorNumero(int numero) {
    // Busqueda secuencial propia por numero de pedido.
    for (int i = 0; i < totalPedidos; i++) {
        if (pedidos[i].numero == numero) return i;
    }
    return -1;
}

void mostrarPedido(const Pedido& p) {
    cout << "\nPedido #" << p.numero
         << " | Cliente: " << p.cliente
         << " | Producto: " << menu[p.producto].nombre
         << " | Cantidad: " << p.cantidad
         << " | Total: Q " << fixed << setprecision(2) << subtotal(p)
         << " | Estado: " << p.estado << '\n';
}

void crearPedido() {
    if (totalPedidos >= MAX_PEDIDOS) {
        cout << "Se alcanzo el limite de pedidos.\n";
        return;
    }

    cout << "\n========== CREAR PEDIDO ==========\n";
    string cliente = leerTexto("Nombre del cliente: ");
    mostrarMenuProductos();
    int producto = leerEntero("Seleccione un producto: ", 1, TOTAL_PRODUCTOS) - 1;
    int cantidad = leerEntero("Cantidad (1-50): ", 1, 50);

    Pedido nuevo;
    nuevo.numero = siguienteNumero++;
    nuevo.cliente = cliente;
    nuevo.producto = producto;
    nuevo.cantidad = cantidad;
    nuevo.estado = "Pendiente";

    pedidos[totalPedidos++] = nuevo;
    guardarPedidos();

    cout << "\nPedido creado correctamente.\n";
    mostrarPedido(nuevo);
}

int prioridadEstado(const string& estado) {
    if (estado == "Pendiente") return 1;
    if (estado == "Preparando") return 2;
    if (estado == "Listo") return 3;
    if (estado == "Entregado") return 4;
    return 5; // Cancelado
}

void ordenarPedidosCocina(int indices[], int cantidad) {
    // Ordenamiento manual Bubble Sort:
    // primero el estado con mayor prioridad y luego el pedido mas antiguo.
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - i - 1; j++) {
            int a = indices[j];
            int b = indices[j + 1];
            bool intercambiar = false;

            int prioridadA = prioridadEstado(pedidos[a].estado);
            int prioridadB = prioridadEstado(pedidos[b].estado);

            if (prioridadA > prioridadB ||
                (prioridadA == prioridadB && pedidos[a].numero > pedidos[b].numero)) {
                intercambiar = true;
            }

            if (intercambiar) swap(indices[j], indices[j + 1]);
        }
    }
}

void mostrarKDS() {
    cout << "\n========== KDS - PANTALLA DE COCINA ==========\n";
    int indices[MAX_PEDIDOS];
    int cantidad = 0;

    for (int i = 0; i < totalPedidos; i++) {
        if (pedidos[i].estado == "Pendiente" ||
            pedidos[i].estado == "Preparando" ||
            pedidos[i].estado == "Listo") {
            indices[cantidad++] = i;
        }
    }

    if (cantidad == 0) {
        cout << "No hay pedidos activos en cocina.\n";
        return;
    }

    ordenarPedidosCocina(indices, cantidad);
    cout << "Orden de atencion: Pendiente -> Preparando -> Listo.\n";
    for (int i = 0; i < cantidad; i++) mostrarPedido(pedidos[indices[i]]);
}

void actualizarPedidoKDS() {
    cout << "\n========== ACTUALIZAR PEDIDO EN KDS ==========\n";
    int numero = leerEntero("Numero de pedido: ", 1, 100000000);
    int posicion = buscarPorNumero(numero);

    if (posicion == -1) {
        cout << "No existe un pedido con ese numero.\n";
        return;
    }

    mostrarPedido(pedidos[posicion]);

    if (pedidos[posicion].estado == "Pendiente") {
        cout << "1. Iniciar preparacion\n2. Cancelar pedido\n";
        int opcion = leerEntero("Seleccione: ", 1, 2);
        pedidos[posicion].estado = (opcion == 1) ? "Preparando" : "Cancelado";
    } else if (pedidos[posicion].estado == "Preparando") {
        cout << "1. Marcar como listo\n2. Cancelar pedido\n";
        int opcion = leerEntero("Seleccione: ", 1, 2);
        pedidos[posicion].estado = (opcion == 1) ? "Listo" : "Cancelado";
    } else if (pedidos[posicion].estado == "Listo") {
        cout << "El pedido ya esta listo. OSS o mostrador puede marcarlo entregado.\n";
        return;
    } else {
        cout << "No se puede modificar un pedido " << pedidos[posicion].estado << ".\n";
        return;
    }

    guardarPedidos();
    cout << "Estado actualizado: " << pedidos[posicion].estado << '\n';
}

void entregarPedido() {
    cout << "\n========== ENTREGA DE PEDIDO ==========\n";
    int numero = leerEntero("Numero de pedido: ", 1, 100000000);
    int posicion = buscarPorNumero(numero);

    if (posicion == -1) {
        cout << "Pedido inexistente.\n";
    } else if (pedidos[posicion].estado != "Listo") {
        cout << "Solo se pueden entregar pedidos que esten en estado Listo.\n";
    } else {
        pedidos[posicion].estado = "Entregado";
        guardarPedidos();
        cout << "Pedido #" << numero << " marcado como Entregado.\n";
    }
}

void moduloKDS() {
    int opcion;
    do {
        cout << "\n========== MODULO KDS ==========\n"
             << "1. Ver pedidos activos y prioridades\n"
             << "2. Actualizar preparacion de un pedido\n"
             << "3. Volver al menu principal\n";
        opcion = leerEntero("Seleccione una opcion: ", 1, 3);

        if (opcion == 1) mostrarKDS();
        else if (opcion == 2) actualizarPedidoKDS();
    } while (opcion != 3);
}

void buscarOSS() {
    cout << "\n========== OSS - CONSULTAR PEDIDO ==========\n"
         << "1. Buscar por numero de pedido\n"
         << "2. Buscar por nombre del cliente\n";
    int opcion = leerEntero("Seleccione el criterio: ", 1, 2);

    if (opcion == 1) {
        int numero = leerEntero("Numero de pedido: ", 1, 100000000);
        int posicion = buscarPorNumero(numero);
        if (posicion == -1) cout << "No se encontro el pedido.\n";
        else mostrarPedido(pedidos[posicion]);
    } else {
        string nombre = minusculas(leerTexto("Nombre o parte del nombre: "));
        bool encontrado = false;
        // Segunda busqueda secuencial: coincidencia por nombre del cliente.
        for (int i = 0; i < totalPedidos; i++) {
            if (minusculas(pedidos[i].cliente).find(nombre) != string::npos) {
                mostrarPedido(pedidos[i]);
                encontrado = true;
            }
        }
        if (!encontrado) cout << "No hay pedidos para ese cliente.\n";
    }
}

void moduloOSS() {
    int opcion;
    do {
        cout << "\n========== MODULO OSS ==========\n"
             << "1. Consultar estado de pedido\n"
             << "2. Marcar pedido listo como entregado\n"
             << "3. Volver al menu principal\n";
        opcion = leerEntero("Seleccione una opcion: ", 1, 3);

        if (opcion == 1) buscarOSS();
        else if (opcion == 2) entregarPedido();
    } while (opcion != 3);
}

void reporteVentas() {
    double totalVentas = 0;
    int pedidosEntregados = 0;

    for (int i = 0; i < totalPedidos; i++) {
        if (pedidos[i].estado == "Entregado") {
            totalVentas += subtotal(pedidos[i]);
            pedidosEntregados++;
        }
    }

    cout << "\n========== REPORTE 1: VENTAS ==========\n"
         << "Pedidos entregados: " << pedidosEntregados << '\n'
         << "Total vendido: Q " << fixed << setprecision(2) << totalVentas << '\n';
}

void reporteEstados() {
    int pendientes = 0, preparando = 0, listos = 0, entregados = 0, cancelados = 0;

    for (int i = 0; i < totalPedidos; i++) {
        if (pedidos[i].estado == "Pendiente") pendientes++;
        else if (pedidos[i].estado == "Preparando") preparando++;
        else if (pedidos[i].estado == "Listo") listos++;
        else if (pedidos[i].estado == "Entregado") entregados++;
        else if (pedidos[i].estado == "Cancelado") cancelados++;
    }

    cout << "\n========== REPORTE 2: PEDIDOS POR ESTADO ==========\n"
         << "Pendientes: " << pendientes << '\n'
         << "En preparacion: " << preparando << '\n'
         << "Listos: " << listos << '\n'
         << "Entregados: " << entregados << '\n'
         << "Cancelados: " << cancelados << '\n'
         << "Total registrado: " << totalPedidos << '\n';
}

void reporteProductos() {
    int vendidos[TOTAL_PRODUCTOS] = {0};

    for (int i = 0; i < totalPedidos; i++) {
        if (pedidos[i].estado == "Entregado") {
            vendidos[pedidos[i].producto] += pedidos[i].cantidad;
        }
    }

    cout << "\n========== REPORTE 3: PRODUCTOS VENDIDOS ==========\n";
    for (int i = 0; i < TOTAL_PRODUCTOS; i++) {
        cout << left << setw(22) << menu[i].nombre
             << "Unidades entregadas: " << vendidos[i] << '\n';
    }
}

void moduloReportes() {
    int opcion;
    do {
        cout << "\n========== REPORTES ==========\n"
             << "1. Reporte de ventas\n"
             << "2. Reporte de pedidos por estado\n"
             << "3. Reporte de productos vendidos\n"
             << "4. Volver al menu principal\n";
        opcion = leerEntero("Seleccione una opcion: ", 1, 4);

        if (opcion == 1) reporteVentas();
        else if (opcion == 2) reporteEstados();
        else if (opcion == 3) reporteProductos();
    } while (opcion != 4);
}

int main() {
    cargarPedidos();

    int opcion;
    do {
        cout << "\n============================================\n"
             << "       RESTAURANTE - SISTEMA KDS Y OSS\n"
             << "============================================\n"
             << "1. Ver menu del restaurante\n"
             << "2. Crear nuevo pedido\n"
             << "3. KDS - Gestion de cocina\n"
             << "4. OSS - Consultar y entregar pedidos\n"
             << "5. Reportes\n"
             << "6. Salir\n";

        opcion = leerEntero("Seleccione una opcion: ", 1, 6);

        switch (opcion) {
            case 1: mostrarMenuProductos(); break;
            case 2: crearPedido(); break;
            case 3: moduloKDS(); break;
            case 4: moduloOSS(); break;
            case 5: moduloReportes(); break;
            case 6:
                guardarPedidos();
                cout << "Datos guardados. Gracias por utilizar el sistema.\n";
                break;
        }
    } while (opcion != 6);

    return 0;
}
