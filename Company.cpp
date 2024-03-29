#include "Company.h"

Company::Company()
{
	N_cargos = new WaitingList;
	S_cargos = new LinkedQueue<Cargos*>;
	V_cargos = new PriorityQueue<Cargos*>;

	N_truckslist_empty = new LinkedQueue<Truck*>;  //normal trucks list
	S_truckslist_empty = new LinkedQueue<Truck*>;  //special trucks list
	V_truckslist_empty = new LinkedQueue<Truck*>;  //VIP trucks list
	
	N_truckslist_loading = new LinkedQueue<Truck*>;  //normal trucks list
	S_truckslist_loading = new LinkedQueue<Truck*>;  //special trucks list
	V_truckslist_loading = new LinkedQueue<Truck*>;  //VIP trucks list

	N_cargos_delivered = new LinkedQueue<Cargos*>;
	S_cargos_delivered = new LinkedQueue<Cargos*>;
	V_cargos_delivered = new LinkedQueue<Cargos*>;

	ListofEvents = new LinkedQueue<Events*>; // List of events
	
	curr_Day = 0;         //Time of current day and hour to take actions in
	curr_Hour = 1;
	NoWaitingCargos = 0;     //Total number of cargos
	NoLoadingTrucks = 0;
	NoEmptyTrucks = 0;
	NoMovingCargos = 0;
	NoInCheckupTrucks = 0;
	NoDeliveredCargos = 0;
}
void Company::AutoPromotion()
{
	Cargos *ptr = (N_cargos->GetEntry(1));
		promotion(ptr->getid());
}

int Company::Getcurr_Day()
{
	return curr_Day;
}
int Company::GetNoDeliveredCargos()
{
	return NoDeliveredCargos;
}
void Company::print_waiting_cargos()
{
	cout << "[";
	N_cargos->PrintList();
	cout << "]";
	cout << "(";
	S_cargos->PrintList();
	cout << ")";
	cout << "{";
	V_cargos->PrintList();
	cout << "}" << endl;
}
void Company::print_empty_truck()
{
	cout << "[";
	N_truckslist_empty->PrintList();
	cout << "]";
	cout << "(";
	S_truckslist_empty->PrintList();
	cout << ")";
	cout << "{";
	V_truckslist_empty->PrintList();
	cout << "}" << endl;

}
void Company::print_delivered_cargos()
{
	cout << "[";
	N_cargos_delivered->PrintList();
	cout << "]";
	cout << "(";
	S_cargos_delivered->PrintList();
	cout << ")";
	cout << "{";
	V_cargos_delivered->PrintList();
	cout << "}" << endl;
}

int  Company::Getcurr_Hour()
{
	return curr_Hour;
}
int Company::GetNoWaitingCargos()
{
	return NoWaitingCargos;
}
int Company::GetNoLoadingTrucks()
{
	return NoLoadingTrucks;
}
int Company::GetNoEmptyTrucks()
{
	return NoEmptyTrucks;
}
int Company::GetNoMovingCargos()
{
	return NoMovingCargos;
}
int Company::GetNoInCheckupTrucks()
{
	return NoInCheckupTrucks;
}
void Company::Run()
{
	UI ui;
	pUI = &ui;
	fileInput = pUI->setMode(this);
	readFile();
	while (true)
	{
		ExecuteEvents();
		NoEmptyTrucks = N_truckslist_empty->getcount() + S_truckslist_empty->getcount() + V_truckslist_empty->getcount();  //this is in phase 1 only
		if (N_cargos->GetEntry(1) && (curr_Day - N_cargos->GetEntry(1)->getprepartionday()) == AutoP && curr_Hour - N_cargos->GetEntry(1)->getprepartionhour() == 0)
		{
			AutoPromotion();
		}
		pUI->Interactive_StepByStep(this);
		curr_Hour++;
		if (curr_Hour == 24)
		{
			curr_Day++;
			curr_Hour = 0;
		}
		if (NoWaitingCargos == 0 && NoLoadingTrucks == 0 && NoMovingCargos == 0 
			&& NoInCheckupTrucks == 0 && ListofEvents->getcount() == 0)
		{
			return;
		}
		FiveSteps();
	}
}

void Company::FiveSteps()
{
	while (true)
	{
		Cargos* test = N_cargos->GetEntry(1);
		if (test)
		{
			int h = 5 + test->getprepartionhour();
			int d = test->getprepartionday();
			if (h >= 24)
			{
				h = h - 24;
				d++;
			}
			if (d == curr_Day && h == curr_Hour)
			{
				N_cargos->DeleteNode(test->getid());
				N_cargos_delivered->enqueue(test);
				NoDeliveredCargos++;
				NoWaitingCargos--;
			}
			else
				break;
		}
		else
			break;
	}
	while (true)
	{
		Cargos* test;
		if (S_cargos->peek(test))
		{
			int h = 5 + test->getprepartionhour();
			int d = test->getprepartionday();
			if (h >= 24)
			{
				h = h - 24;
				d++;
			}
			if (d == curr_Day && h == curr_Hour)
			{
				S_cargos->dequeue(test);
				S_cargos_delivered->enqueue(test);
				NoDeliveredCargos++;
				NoWaitingCargos--;
			}
			else
				break;
		}
		else
			break;
	}
	while (true)
	{
		Cargos* test;
		if (V_cargos->peekFront(test))
		{
			int h = 5 + test->getprepartionhour();
			int d = test->getprepartionday();
			if (h >= 24)
			{
				h = h - 24;
				d++;
			}
			if (d <= curr_Day && h <= curr_Hour)
			{
				V_cargos->dequeue(test);
				V_cargos_delivered->enqueue(test);
				NoDeliveredCargos++;
				NoWaitingCargos--;
			}
			else
				break;
		}
		else
			break;
	}
}

void Company::setFileInput(string file)
{
	fileInput = file;
}

void Company::readFile()
{
	int N, S, V, NS, SS, VS, NTC, STC, VTC, J, CN, CS, CV, E, ETDay, ETHours, 
		ID, DIST, LT, Cost,
		EDDay, EDHours, ExtraMoney;
	char TYP, evnt;
	ifstream file(fileInput, ios::in); 
		file >> N;  // number of normal trucks
		file >> S;  // number of special trucks
		file >> V;  // number of VIP trucks
		file >> NS;  //speed of normal trucks
		file >> SS;  //speed of special trucks
		file >> VS;  //speed of VIP trucks
		file >> NTC;  //normal truck capacity
		file >> STC;  //normal special capacity
		file >> VTC;  //normal VIP capacity
		file >> J;  //number of journeys the truck completes before performing a checkup
		file >> CN;  //check up duration for normal trucks
		file >> CS;  //check up duration for special trucks
		file >> CV;  //check up duration for VIP trucks
		for (int i = 0; i < N; i++)
		{
			Truck* a= new Truck ('N',NS,NTC,J,CN,i+1); 
			N_truckslist_empty->enqueue(a);
		}
		for (int i = 0; i < S; i++)
		{
			Truck* b = new Truck('S', SS, STC, J, CS, N + i + 1);
			S_truckslist_empty->enqueue(b);
		}
		for (int i = 0; i < V; i++)
		{
			Truck* c =new Truck('V', VS, VTC, J, CV, N + S + i + 1);
			V_truckslist_empty->enqueue(c);
		}
		file >> AutoP;

		file >> MaxW;

		file >> E;  //number of events
		for (int i = 0; i < E; i++)
		{
			file >> evnt; 
			if (evnt == 'R')
			{
				file >> TYP;  
				file >> ETDay;
				file.ignore();
				file >> ETHours;
				file >> ID;
				file >> DIST;
				file >> LT;
				file >> Cost;
				ReadyEvents* ready = new ReadyEvents(TYP, ETDay, ETHours, ID, DIST, LT, Cost); 
				ListofEvents->enqueue(ready);
			}
			else if (evnt == 'X')
			{
				file >> EDDay;
				file.ignore();
				file >> EDHours;
				file >> ID;
				CancellationEvents* cancel =new CancellationEvents(EDDay, EDHours, ID);
				ListofEvents->enqueue(cancel);
			}
			else if (evnt == 'P')
			{
				file >> EDDay;
				file.ignore();
				file >> EDHours;
				file >> ID;
				file >> ExtraMoney;
				PromotionEvents* promotion= new PromotionEvents(EDDay, EDHours, ID, ExtraMoney);
				ListofEvents->enqueue(promotion);
			}
		}
}

void Company::addtoline(Cargos* c, int prio)
{
	char x;
	x = c->gettype();
	NoWaitingCargos++;
	if (x == 'N')
	{
		N_cargos->InsertEnd(c);
		N_cargos_count++;
	}
	else if (x == 'S')
	{
		S_cargos->enqueue(c);
		S_Cargos_count++;
	}
	else if (x == 'V')
	{
		V_cargos->enqueue(c,prio);
		V_cargos_count++;
	}
}

int Company::calcprio(Cargos* c)
{
	return c->getcost() + c->getdelivery_dist() + c->getloadtime();
}

void Company::cancellationcargo(int id)
{
	N_cargos->DeleteNode(id);
	N_cargos_count--;
	NoWaitingCargos--;
}

void Company::promotion(int id)
{
	Cargos* pc = N_cargos->Find(id);
	pc->settype('V');
	int prio=calcprio(pc);
	cancellationcargo(id);
	/*pc->setcost(pc->getcost() +);*/
	addtoline(pc,prio);
	V_cargos_count++;
}

void Company::ExecuteEvents()
{
	Events* ptrEvent;
	while (ListofEvents->peek(ptrEvent))
	{
		if ((ptrEvent->getetday() == curr_Day) && (ptrEvent->getethour() == curr_Hour))
		{
			ptrEvent->execute(this);
			ListofEvents->dequeue(ptrEvent);
			delete ptrEvent;
			ptrEvent = NULL;
		}
		else return;
	}
}

Company::~Company()
{
	delete N_cargos;
	delete S_cargos;
	delete V_cargos;

	delete N_truckslist_empty;  //normal trucks list
	delete S_truckslist_empty;  //special trucks list
	delete V_truckslist_empty;  //VIP trucks list

	delete N_truckslist_loading;  //normal trucks list
	delete S_truckslist_loading;  //special trucks list
	delete V_truckslist_loading;  //VIP trucks list
	
	delete N_cargos_delivered;
	delete S_cargos_delivered;
	delete V_cargos_delivered;

	delete ListofEvents;
}
