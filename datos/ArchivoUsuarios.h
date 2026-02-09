#ifndef ARCHIVO_USUARIOS_H
#define ARCHIVO_USUARIOS_H

#include <string>
#include <vector>
#include "../modelos/Usuario.h"

namespace pos {

class ArchivoUsuarios {
public:
    explicit ArchivoUsuarios(const std::string& ruta);

    bool cargar(std::vector<Usuario>& usuarios);
    bool guardar(const std::vector<Usuario>& usuarios);

private:
    std::string rutaArchivo;
};

}

#endif