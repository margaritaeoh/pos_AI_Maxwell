#ifndef ARCHIVO_VENTAS_H
#define ARCHIVO_VENTAS_H

#include <string>
#include <vector>
#include "../modelos/Venta.h"

namespace pos {

class ArchivoVentas {
public:
    explicit ArchivoVentas(const std::string& ruta);

    bool guardarVenta(const Venta& venta);
    bool cargarVentas(std::vector<Venta>& ventas);

private:
    std::string rutaArchivo;
};

}

#endif