#include "ArchivoVentas.h"
#include <fstream>
#include <sstream>

namespace pos {

ArchivoVentas::ArchivoVentas(const std::string& ruta)
    : rutaArchivo(ruta) {}

bool ArchivoVentas::guardarVenta(const Venta& venta) {
    std::ofstream out(rutaArchivo, std::ios::app);
    if (!out.is_open()) return false;

    out << "VENTA," << venta.id << "," << venta.fechaHora << ","
        << venta.usuario << "," << venta.total << "\n";

    for (const auto& l : venta.lineas) {
        out << "LINEA," << venta.id << ","
            << l.productoId << ","
            << l.cantidad << ","
            << l.precioUnitario << ","
            << l.subtotal << "\n";
    }

    return true;
}

bool ArchivoVentas::cargarVentas(std::vector<Venta>& ventas) {
    ventas.clear();

    std::ifstream in(rutaArchivo);
    if (!in.is_open()) return false;

    std::string linea;
    Venta actual;
    bool enVenta = false;

    while (std::getline(in, linea)) {
        std::stringstream ss(linea);
        std::string tipo;
        std::getline(ss, tipo, ',');

        if (tipo == "VENTA") {
            if (enVenta) ventas.push_back(actual);

            enVenta = true;
            actual = Venta{};
            std::string campo;

            std::getline(ss, campo, ','); actual.id = std::stoll(campo);
            std::getline(ss, actual.fechaHora, ',');
            std::getline(ss, actual.usuario, ',');
            std::getline(ss, campo, ','); actual.total = std::stod(campo);
        }
        else if (tipo == "LINEA") {
            LineaVenta l;
            std::string campo;

            std::getline(ss, campo, ','); /* idVenta ignorado */
            std::getline(ss, campo, ','); l.productoId = std::stoll(campo);
            std::getline(ss, campo, ','); l.cantidad = std::stod(campo);
            std::getline(ss, campo, ','); l.precioUnitario = std::stod(campo);
            std::getline(ss, campo, ','); l.subtotal = std::stod(campo);

            actual.lineas.push_back(l);
        }
    }

    if (enVenta) ventas.push_back(actual);

    return true;
}

}