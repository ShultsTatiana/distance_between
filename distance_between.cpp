#include <iostream>
#include <vector>
#include <iterator>

#include <sstream>
using namespace std;

//в худшем случае 	10^6 		+ 		10^6 		+ 		10^6 		= 3*10^6 тактов
//			   считывание_домов + запись_расстояний + вывод_результата
//доступно ~800*10^6 тактов
//Пожалуй, можно было бы и разлить считывание_домов и перезапись_расстояний на фукции :/

template <typename Iterator>
void setDistance(Iterator start, Iterator finish) {
	int distance(1); //не имеет смысла переписывать 0 у пустых участков
	for (; start != finish; ++start) {
		*start = distance++;
	}
}

vector<int> readData(istream& input = std::cin) {
	int streetLength(0); //n - длинна улицы
	input >> streetLength;

	if (streetLength == 1) { return { 0 }; } //1 дом - один 0

	vector<int> street(streetLength); //~16 or 32 * 10^6 -  2 or 4 mb
	//vector - быстрый доступ к элементам, рандомные итераторы

	int lastEmpty(-1); //ни один пустой участок пока не найден
	//помним, что нам гарантировали, что пустой участок есть


	//Экономим время на считывании делая одновременную обработку пустых участков
	// и перезапись номера дома на расстояние до ближайшего пустого дома
	//Плохо - код не разделён на блоки по функциям
	for (int house(0); house < streetLength; ++house) {
		input >> street[house]; //читаем номер дома

		if (street[house] == 0) { //участок пустой? - конец интервала найден
			if (house != 0) { //это первый участок в начале улицы?  
				int reversI = streetLength - 1 - house; //позиция участка с конца
				//конец интервала измерения и перезаписи расстояний - начало конца) 
				auto rstart = street.rbegin() + reversI + 1;

				if (lastEmpty == -1) { //в начале интервала не было пустого участка?
					//меняем номера домов на расстояния с конца интервала к началу "улицы"
					setDistance(rstart, street.rend());
				}
				else { //обработает интервал с пустыми участками в начале и конце
					// начало интервала измерения и перезаписи расстояний - начало начала)
					auto start = street.begin() + lastEmpty + 1;
					//считаем расстояние между пустыми участками (0-1-0) и 
					// игнорируем интервалы типа 0-0
					if (int distance = house - lastEmpty - 1; distance > 0) {
						//меняем номера домов на расстояние до ближайшего пустого участка

						//для домов ближе к началу интервала: с начала начал
						setDistance(start, start + (distance / 2) + (distance % 2));
						//для домов ближе к концу интервала: с начала конца
						setDistance(rstart, rstart + (distance / 2));

						//все до середины, учитывая нечетность расстояния
					}
				}
			}
			lastEmpty = house; //перезаписываем начало следующего интервала
		}
		else if (house == streetLength - 1) { //это последний участок и он не пуст?
			//переписываем номера на расстояния от начала интервала к концу "улицы"
			setDistance(street.begin() + lastEmpty + 1, street.end());
		}
	}

	return street; //vector<int> street - NRVO
}

//отправляем результаты работы программы в поток
ostream& writeData(vector<int> result, ostream& out = std::cout) {
	for (int element : result) {
		out << element << " ";
	}
	return out;
}

//чуть-чуть автоматизирую тестирование функции
struct Test {
	string input;
	string expect;
};

void tests() {
	vector<Test> tests{
		{"1\n0\n", "0 "},	//0
		{"2\n0 1\n", "0 1 "},	//1
		{"2\n1 0\n", "1 0 "},	//2
		{"2\n0 0\n", "0 0 "},	//3
		{"3\n0 0 0\n", "0 0 0 "},	//4
		{"3\n0 2 1\n", "0 1 2 "},	//5
		{"3\n1 0 2\n", "1 0 1 "},	//6
		{"3\n1 2 0\n", "2 1 0 "},	//7
		{"10\n0 1 2 0 3 4 0 5 6 0\n", "0 1 1 0 1 1 0 1 1 0 "}, //8
		{"10\n7 1 2 0 3 4 0 5 6 0\n", "3 2 1 0 1 1 0 1 1 0 "}, //9
		{"10\n0 1 2 0 3 4 0 5 6 7\n", "0 1 1 0 1 1 0 1 2 3 "}, //10
		{"11\n0 1 2 0 3 4 7 0 5 6 0\n",	 "0 1 1 0 1 2 1 0 1 1 0 " }, //11
		{"11\n0 1 2 8 3 4 7 9 5 6 10\n", "0 1 2 3 4 5 6 7 8 9 10 "}, //12
		{"11\n10 1 2 8 3 4 7 9 5 6 0\n", "10 9 8 7 6 5 4 3 2 1 0 "}, //13
		{"11\n4 1 2 8 3 0 7 9 5 6 10\n", "5 4 3 2 1 0 1 2 3 4 5 " }, //14
		{"9\n1 2 8 3 0 7 4 5 6\n", "4 3 2 1 0 1 2 3 4 "}, //15
		{"10\n1 2 3 4 5 6 7 8 9 0\n", "9 8 7 6 5 4 3 2 1 0 "}, //16
		{"10\n0 1 0 2 0 3 0 4 0 5\n", "0 1 0 1 0 1 0 1 0 1 "}, //17
		{"10\n0 0 0 0 0 0 0 0 0 0\n", "0 0 0 0 0 0 0 0 0 0 "}, //18
		{"10\n0 0 0 0 0 1 3 2 5 4\n", "0 0 0 0 0 1 2 3 4 5 "}, //19
		{"10\n3 5 4 2 1 0 0 0 0 0\n", "5 4 3 2 1 0 0 0 0 0 "}, //20
		{"13\n6 4 2 0 1 3 5 7 9 0 8 10 0\n", "3 2 1 0 1 2 3 2 1 0 1 1 0 "}, //21
		{"12\n6 4 2 0 1 3 7 9 0 8 5 0\n", "3 2 1 0 1 2 2 1 0 1 1 0 "},//22
	};

	size_t failCounter(0), line(0);
  //проходим по тестовым вводам
	for (Test test : tests) {
    //строка ввода больше не понадобится - перемещаем ее в поток
		stringstream input(move(test.input));
		stringstream out;
    
    //считываем поток readData-ой и отправляем результат на запись в поток out
		writeData(readData(input), out);
    //
		string strOutput;
		getline(out, strOutput);
    
    //сравниваем тестовый ожидаемое с полученным
		if (strOutput != test.expect) {
			++failCounter;
			std::cout << "Test in " << line << "\tis\t" <<
				strOutput << "!= " << test.expect << endl;
		}
		++line;
	}
	std::cout << (failCounter == 0 ? "All tests is Ok\n" : "FAIL\n");
}

int main()
{
	tests();

	writeData(readData());

	return 0;
}

