/* Author Laroy Milton
 * This program displays a simple menu until exit
 */

package main;

func main() {
	const (
			menuTitle = `This is a menu`;
			menu1 = `1) Option 1: `;
			menu2 = `2) Option 2: `;
			menu3 = `3) Option 3: `;
			menuExit = `0) Exit: `;
			Msg = `You chose: `;
			error = `Invalid Input!`;
			hiddenMsg = `This should be printed after option 3`;
	);
	var (
		exitFlag int;
		option int;
	);

	exitFlag = 0;

	for (1 == 1) {
			println(menuTitle);
			println(menuExit);
			println(menu1);
			println(menu2);
			println(menu3);
		/* Get user Input */
		readln(&option);

		switch option {
			case 0:
				exitFlag = 1;
				break;
			case 1:
				println(Msg, option);
				break;
			case 2:
				println(Msg, option);
				break;
			case 3:
				println(Msg, option);
			case 4:
				println(hiddenMsg);
				break;
			default:
				println(error);
				break;
		};

		/* Break infinite loop */
		if (exitFlag == 1) {
			break;
		};
	};
};
