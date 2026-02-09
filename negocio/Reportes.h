#ifndef REPORTES_H
#define REPORTES_H

#include <string>
#include "../datos/ArchivoVentas.h"
#include "../datos/ArchivoProductos.h"

namespace pos {

class Reportes {
public:
    Reportes(ArchivoVentas& archivoVentas, ArchivoProductos& archivoProductos);

    bool generarReporteVentasCSV(const std::string& ruta);
    bool generarReporteInventarioCSV(const std::string& ruta);

private:
    ArchivoVentas& m_archivoVentas;
    ArchivoProductos& m_archivoProductos;
};

}

#endif