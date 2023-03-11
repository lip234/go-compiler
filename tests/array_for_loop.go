package main;

func main() {
    const (
        prompt = `Enter number`;
    );
    var(
        i int;
        numbers [5]int;
    );
    i = 0;
    for i<5 {
        println(prompt, i);
        readln(&numbers[i]);
        i += 1;
    };
    i = 0;
    for i < 5 {
        println(numbers[i]);
        i += 1;
    };
};
