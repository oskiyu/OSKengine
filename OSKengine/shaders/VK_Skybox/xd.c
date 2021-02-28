
int main(){

    //Da error, el compilador no sabe que hay una función llamada XD.
    XD();

    return 0;
}

void XD(){

}

///////////////////////////////

void XD(){

}
int main(){

    //NO da error, el compilador ya sabe que hay una función llamada XD,
    //porque lo leyó encima.
    XD();

    return 0;
}

////////////////////////

void XD();

int main(){

    //NO da error, el compilador ya sabe que hay una función llamada XD,
    //porque lo leyó encima. No sabe lo que hace, pero sabe que existe.
    XD();

    return 0;
}

void XD(){

}

