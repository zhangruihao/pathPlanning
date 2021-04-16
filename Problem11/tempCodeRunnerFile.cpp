t generation = 100;  //迭代次数
    for (int i = 0; i <= generation; i++) {
        cout << i << endl;
        ga.evolve();
    }
    ga.