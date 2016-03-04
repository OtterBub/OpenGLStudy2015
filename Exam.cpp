int main()
{
	GameSysInit();

	GameInit();

	while(1)
	{

		Input();

		GameUpdate();

		GameDraw();

		// Exit
		//break;
	}

	return 0;
}