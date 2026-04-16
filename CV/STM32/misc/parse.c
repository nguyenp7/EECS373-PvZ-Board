buffer[idx] = '\0';

if (idx > 0)
{
    char color[16];
    int row;
    int col;

    if (sscanf(buffer, "%15[^,],%d,%d", color, &row, &col) == 3)
    {
        char out[80];
        snprintf(out, sizeof(out), "Parsed -> color=%s row=%d col=%d\r\n", color, row, col);
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)out, strlen(out), HAL_MAX_DELAY);
    }
    else
    {
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Parse error\r\n", 13, HAL_MAX_DELAY);
    }
}

idx = 0;