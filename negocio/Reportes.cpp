#include "Reportes.h"
#include <fstream>

namespace pos {

Reportes::Reportes(ArchivoVentas& archivoVentas, ArchivoProductos& archivoProductos)
    : m_archivoVentas(archivoVentas),
      m_archivoProductos(archivoProductos) {}

bool Reportes::generarReporteVentasCSV(const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) return false;

    out << "idVenta,fechaHora,usuario,productoId,cantidad,precioUnitario,subtotal,totalVenta\n";

    std::vector<Venta> ventas;
    if (!m_archivoVentas.cargarVentas(ventas)) {
        return false;
    }

    for (const auto& v : ventas) {
        for (const auto& l : v.lineas) {
            out << v.id << ","
                << v.fechaHora << ","
                << v.usuario << ","
                << l.productoId << ","
                << l.cantidad << ","
                << l.precioUnitario << ","
                << l.subtotal << ","
                << v.total << "\n";
        }
    }

    return true;
}

bool Reportes::generarReporteInventarioCSV(const std::string& ruta) {
    std::ofstream out(ruta);
    if (!out.is_open()) return false;

    out << "id,codigoBarras,codigoLocal,nombre,precioPublico,precioMayorista,cantidad,presentacion\n";

    std::vector<Producto> productos;
    if (!m_archivoProductos.cargar(productos)) {
        return false;
    }

    for (const auto& p : productos) {
        out << p.id << ","
            << p.codigoBarras << ","
            << p.codigoLocal << ","
            << p.nombre << ","
            << p.precioPublico << ","
            << p.precioMayorista << ","
            << p.cantidadInventario << ","
            << (int)p.presentacion << "\n";
    }

    return true;
}

}