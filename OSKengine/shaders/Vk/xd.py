
segundosTotales = 15654

minutos = segundosTotales / 60
segundos = (minutos - int(minutos)) * 60
minutos = float(int(minutos))

horas = minutos / 60
minutos = (horas - int(horas)) * 60
horas = float(int(horas))


def fraseConMasAes(listaFrases):
    if len(listaFrases) == 0:
        return ""

    numero = numeroDeAes(listaFrases[0])
    numero2 = numeroDeAes( fraseConMasAes(listaFrases[1:]) )

    if (numero > numero2)
        return listaFrases[0]
    else:
        return fraseConMasAes(listaFrases[1:])

    