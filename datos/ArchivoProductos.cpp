#include "ArchivoProductos.h"
#include <fstream>
#include <sstream>
#include <wx/wx.h>

namespace pos {

// Helper: convert system encoding to wxString (which handles UTF-8 properly)
static wxString toWxString(const std::string& str) {
    // Attempt to interpret as UTF-8 first, fall back to system encoding
    wxString result = wxString::FromUTF8(str.c_str());
    if (result.IsEmpty() && !str.empty()) {
        // If UTF-8 decode failed, use the default locale conversion
        result = wxString(str.c_str(), wxConvISO8859_1);
    }
    return result;
}

ArchivoProductos::ArchivoProductos(const std::string& ruta)
    : rutaArchivo(ruta) {}

bool ArchivoProductos::cargar(std::vector<Producto>& productos) {
    productos.clear();

    std::ifstream in(rutaArchivo);
    if (!in.is_open()) return false;

    std::string linea;
    std::getline(in, linea); // encabezado

    while (std::getline(in, linea)) {
        std::stringstream ss(linea);
        std::string campo;
        Producto p;

        std::getline(ss, campo, ','); p.id = std::stoll(campo);
        std::getline(ss, p.codigoBarras, ',');
        std::getline(ss, p.codigoLocal, ',');
        std::getline(ss, p.nombre, ',');
        std::getline(ss, p.descripcion, ',');
        std::getline(ss, campo, ','); p.precioPublico = std::stod(campo);
        std::getline(ss, campo, ','); p.precioMayorista = std::stod(campo);
        std::getline(ss, campo, ','); p.tipoUnidad = (TipoUnidad)std::stoi(campo);
        std::getline(ss, campo, ','); p.cantidadInventario = std::stod(campo);
        std::getline(ss, campo, ','); p.anioCaducidad = std::stoi(campo);
        std::getline(ss, campo, ','); p.mesCaducidad = std::stoi(campo);
        std::getline(ss, campo, ','); p.diaCaducidad = std::stoi(campo);
        std::getline(ss, campo, ','); p.presentacion = (TipoPresentacion)std::stoi(campo);
        std::getline(ss, campo, ','); p.piezasPorPresentacion = std::stoi(campo);

        productos.push_back(p);
    }

    return true;
}

bool ArchivoProductos::guardar(const std::vector<Producto>& productos) {
    std::ofstream out(rutaArchivo);
    if (!out.is_open()) return false;

    out << "id,codigoBarras,codigoLocal,nombre,descripcion,precioPublico,precioMayorista,"
           "tipoUnidad,cantidadInventario,anioCaducidad,mesCaducidad,diaCaducidad,presentacion,piezasPorPresentacion\n";

    for (const auto& p : productos) {
        out << p.id << ","
            << p.codigoBarras << ","
            << p.codigoLocal << ","
            << p.nombre << ","
            << p.descripcion << ","
            << p.precioPublico << ","
            << p.precioMayorista << ","
            << (int)p.tipoUnidad << ","
            << p.cantidadInventario << ","
            << p.anioCaducidad << ","
            << p.mesCaducidad << ","
            << p.diaCaducidad << ","
            << (int)p.presentacion << ","
            << p.piezasPorPresentacion << "\n";
    }

    return true;
}

}