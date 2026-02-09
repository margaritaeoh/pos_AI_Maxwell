#ifndef ARCHIVO_PRODUCTOS_H
#define ARCHIVO_PRODUCTOS_H

#include <string>
#include <vector>
#include "../modelos/Producto.h"

namespace pos {

class ArchivoProductos {
public:
    explicit ArchivoProductos(const std::string& ruta);

    bool cargar(std::vector<Producto>& productos);
    bool guardar(const std::vector<Producto>& productos);

private:
    std::string rutaArchivo;
};

}

#endif